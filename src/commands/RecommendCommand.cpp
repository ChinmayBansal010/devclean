#include "commands/RecommendCommand.hpp"

#include "core/CacheInsights.hpp"
#include "core/Config.hpp"
#include "core/ScanHistory.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <nlohmann/json.hpp>

namespace {

std::string normalize(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string canonicalCategory(const std::string& value)
{
    const std::string normalized = normalize(value);
    if (normalized == "node")
        return "javascript";
    if (normalized == "package-managers")
        return "package managers";
    return normalized;
}

std::vector<ScanResult> applyFilters(std::vector<ScanResult> results, const ParsedArgs& args)
{
    if (args.activeOnly)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [](const ScanResult& result) {
            return !result.active;
        }), results.end());
    }

    if (args.minSizeBytes > 0)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return !result.found || result.bytes < args.minSizeBytes;
        }), results.end());
    }

    if (args.maxSizeBytes > 0)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return !result.found || result.bytes > args.maxSizeBytes;
        }), results.end());
    }

    if (!args.category.empty())
    {
        const std::string category = canonicalCategory(args.category);
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return canonicalCategory(result.category) != category;
        }), results.end());
    }

    if (!args.excludes.empty())
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            const std::string normalizedName = normalize(result.name);
            return std::any_of(args.excludes.begin(), args.excludes.end(), [&](const std::string& excluded) {
                return normalizedName == normalize(excluded);
            });
        }), results.end());
    }

    return results;
}

void printTextReport(const CacheInsights& insights)
{
    std::cout << "Recommendations\n";
    std::cout << "---------------\n";
    std::cout << "Health score: " << insights.health.score << "/100 (" << insights.health.label << ")\n";
    std::cout << "Total usage: " << Formatter::formatBytes(insights.totalBytes) << '\n';
    std::cout << "Caches found: " << insights.foundCount << '\n';
    std::cout << "Active caches: " << insights.activeCount << '\n';
    std::cout << "Warnings: " << insights.warningCount << '\n';
    if (!insights.history.empty())
    {
        std::cout << "Latest growth: " << (insights.latestGrowthBytes >= 0 ? "+" : "-")
                  << Formatter::formatBytes(static_cast<uint64_t>(insights.latestGrowthBytes >= 0 ? insights.latestGrowthBytes : -insights.latestGrowthBytes))
                  << " (" << (insights.latestGrowthPercent >= 0.0 ? "+" : "") << insights.latestGrowthPercent << "%)\n";
        std::cout << "Average growth: " << (insights.averageGrowthBytes >= 0 ? "+" : "-")
                  << Formatter::formatBytes(static_cast<uint64_t>(insights.averageGrowthBytes >= 0 ? insights.averageGrowthBytes : -insights.averageGrowthBytes))
                  << " per snapshot\n";
    }

    std::cout << "\nTop recommendations:\n";
    if (insights.recommendations.empty())
    {
        std::cout << "  - no strong cleanup recommendations at this time\n";
        return;
    }

    const std::size_t limit = std::min<std::size_t>(insights.recommendations.size(), 8);
    for (std::size_t i = 0; i < limit; ++i)
    {
        const auto& recommendation = insights.recommendations[i];
        std::cout << "  - " << recommendation.name << " (" << Formatter::formatBytes(recommendation.bytes) << ")\n";
        std::cout << "    " << recommendation.reason << '\n';
        std::cout << "    Action: " << recommendation.action << '\n';
    }
}

void printJsonReport(const CacheInsights& insights)
{
    nlohmann::json payload = nlohmann::json::object();
    payload["command"] = "recommend";
    payload["health"] = {
        {"score", insights.health.score},
        {"label", insights.health.label},
        {"factors", insights.health.factors}
    };
    payload["totals"] = {
        {"bytes", insights.totalBytes},
        {"files", insights.totalFiles},
        {"directories", insights.totalDirectories},
        {"found_count", insights.foundCount},
        {"active_count", insights.activeCount},
        {"warning_count", insights.warningCount}
    };
    payload["trend"] = {
        {"latest_growth_bytes", insights.latestGrowthBytes},
        {"latest_growth_percent", insights.latestGrowthPercent},
        {"average_growth_bytes", insights.averageGrowthBytes}
    };
    payload["recommendations"] = nlohmann::json::array();
    for (const auto& recommendation : insights.recommendations)
    {
        payload["recommendations"].push_back({
            {"name", recommendation.name},
            {"reason", recommendation.reason},
            {"action", recommendation.action},
            {"bytes", recommendation.bytes},
            {"growth_bytes", recommendation.growthBytes},
            {"priority", recommendation.priority},
            {"safe", recommendation.safe}
        });
    }
    std::cout << payload.dump(2) << '\n';
}

} // namespace

int RecommendCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ParsedArgs effectiveArgs = args;
    if (effectiveArgs.category.empty() && !config.defaultCategory.empty())
        effectiveArgs.category = config.defaultCategory;

    ScannerEngine scanner;
    auto results = scanner.scan(effectiveArgs.targets, config);
    results = applyFilters(std::move(results), effectiveArgs);
    ScanHistory::getInstance().recordScan(results);
    CacheInsights insights = buildCacheInsights(results, ScanHistory::getInstance().getHistory(6));

    if (args.json)
    {
        printJsonReport(insights);
        return 0;
    }

    printTextReport(insights);
    return 0;
}
