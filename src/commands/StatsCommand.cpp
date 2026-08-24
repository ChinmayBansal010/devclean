#include "commands/StatsCommand.hpp"

#include "core/CacheInsights.hpp"
#include "core/Config.hpp"
#include "core/ScanHistory.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace {

using CategoryStat = std::pair<std::string, std::pair<uint64_t, std::size_t>>;

uint64_t saturatingAdd(uint64_t lhs, uint64_t rhs)
{
    if (rhs > std::numeric_limits<uint64_t>::max() - lhs)
        return std::numeric_limits<uint64_t>::max();
    return lhs + rhs;
}

std::vector<CategoryStat> sortCategories(
    const std::map<std::string, uint64_t>& categoryBytes,
    const std::map<std::string, std::size_t>& categoryCounts)
{
    std::vector<CategoryStat> categories;
    categories.reserve(categoryBytes.size());

    for (const auto& [category, bytes] : categoryBytes)
        categories.emplace_back(category, std::make_pair(bytes, categoryCounts.at(category)));

    std::stable_sort(categories.begin(), categories.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.second.first != rhs.second.first)
            return lhs.second.first > rhs.second.first;
        return lhs.first < rhs.first;
    });

    return categories;
}

void applyFilters(std::vector<ScanResult>& results, const ParsedArgs& args)
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
}

}

int StatsCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ScannerEngine scanner;
    auto results = scanner.scan(args.targets, config);
    applyFilters(results, args);
    ScanHistory::getInstance().recordScan(results);
    CacheInsights insights = buildCacheInsights(results, ScanHistory::getInstance().getHistory(6));

    uint64_t totalBytes = 0;
    uint64_t totalFiles = 0;
    uint64_t totalDirectories = 0;
    std::size_t foundCount = 0;
    std::size_t activeCount = 0;
    std::map<std::string, uint64_t> categoryBytes;
    std::map<std::string, std::size_t> categoryCounts;
    std::vector<std::pair<std::string, uint64_t>> largestCaches;

    for (const auto& result : results)
    {
        totalBytes = saturatingAdd(totalBytes, result.bytes);
        totalFiles = saturatingAdd(totalFiles, result.files);
        totalDirectories = saturatingAdd(totalDirectories, result.directories);
        if (result.active)
            ++activeCount;
        if (result.found)
        {
            ++foundCount;
            const std::string category = result.category.empty() ? "misc" : result.category;
            categoryBytes[category] = saturatingAdd(categoryBytes[category], result.bytes);
            ++categoryCounts[category];
            largestCaches.emplace_back(result.name, result.bytes);
        }
    }

    std::stable_sort(largestCaches.begin(), largestCaches.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.second != rhs.second)
            return lhs.second > rhs.second;
        return lhs.first < rhs.first;
    });

    const auto categories = sortCategories(categoryBytes, categoryCounts);

    if (args.json)
    {
        nlohmann::json payload = nlohmann::json::object();
        payload["command"] = "stats";
        payload["cache_count"] = results.size();
        payload["found_count"] = foundCount;
        payload["active_count"] = activeCount;
        payload["inactive_count"] = results.size() - activeCount;
        payload["active_only"] = args.activeOnly;
        payload["min_size_bytes"] = args.minSizeBytes;
        payload["max_size_bytes"] = args.maxSizeBytes;
        payload["total_bytes"] = totalBytes;
        payload["total_files"] = totalFiles;
        payload["total_directories"] = totalDirectories;
        payload["health"] = {
            {"score", insights.health.score},
            {"label", insights.health.label},
            {"factors", insights.health.factors}
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
        payload["largest_caches"] = nlohmann::json::array();
        for (std::size_t i = 0; i < std::min<std::size_t>(largestCaches.size(), 5); ++i)
        {
            nlohmann::json entry = nlohmann::json::object();
            entry["name"] = largestCaches[i].first;
            entry["bytes"] = largestCaches[i].second;
            payload["largest_caches"].push_back(std::move(entry));
        }
        payload["largest_categories"] = nlohmann::json::array();
        for (const auto& [category, stat] : categories)
        {
            nlohmann::json entry = nlohmann::json::object();
            entry["name"] = category;
            entry["bytes"] = stat.first;
            entry["count"] = stat.second;
            payload["largest_categories"].push_back(std::move(entry));
        }
        std::cout << payload.dump(2) << '\n';
        return 0;
    }

    std::cout << "Statistics\n";
    std::cout << "----------\n";
    std::cout << "Filters: ";
    if (!args.activeOnly && args.minSizeBytes == 0 && args.maxSizeBytes == 0)
        std::cout << "none\n";
    else
    {
        bool needsSeparator = false;
        if (args.activeOnly)
        {
            std::cout << "active-only";
            needsSeparator = true;
        }
        if (args.minSizeBytes > 0)
        {
            if (needsSeparator)
                std::cout << ", ";
            std::cout << "min=" << Formatter::formatBytes(args.minSizeBytes);
            needsSeparator = true;
        }
        if (args.maxSizeBytes > 0)
        {
            if (needsSeparator)
                std::cout << ", ";
            std::cout << "max=" << Formatter::formatBytes(args.maxSizeBytes);
        }
        std::cout << '\n';
    }
    std::cout << "Caches scanned: " << results.size() << '\n';
    std::cout << "Caches found: " << foundCount << '\n';
    std::cout << "Active tools: " << activeCount << '\n';
    std::cout << "Inactive tools: " << results.size() - activeCount << '\n';
    std::cout << "Total size: " << Formatter::formatBytes(totalBytes) << '\n';
    std::cout << "Total files: " << totalFiles << '\n';
    std::cout << "Total directories: " << totalDirectories << '\n';
    std::cout << "Health score: " << insights.health.score << "/100 (" << insights.health.label << ")\n";
    std::cout << "Latest growth: "
              << (insights.latestGrowthBytes >= 0 ? "+" : "-")
              << Formatter::formatBytes(static_cast<uint64_t>(insights.latestGrowthBytes >= 0 ? insights.latestGrowthBytes : -insights.latestGrowthBytes))
              << " (" << (insights.latestGrowthPercent >= 0.0 ? "+" : "") << insights.latestGrowthPercent << "%)\n";
    std::cout << "Average growth: "
              << (insights.averageGrowthBytes >= 0 ? "+" : "-")
              << Formatter::formatBytes(static_cast<uint64_t>(insights.averageGrowthBytes >= 0 ? insights.averageGrowthBytes : -insights.averageGrowthBytes))
              << " per snapshot\n";
    if (!insights.recommendations.empty())
        std::cout << "Top recommendation: " << insights.recommendations.front().name << " - " << insights.recommendations.front().action << '\n';
    std::cout << "\nLargest caches:\n";
    for (std::size_t i = 0; i < std::min<std::size_t>(largestCaches.size(), 5); ++i)
        std::cout << "  - " << largestCaches[i].first << " (" << Formatter::formatBytes(largestCaches[i].second) << ")\n";
    std::cout << "\nLargest categories:\n";
    for (const auto& [category, stat] : categories)
        std::cout << "  - " << category << " (" << Formatter::formatBytes(stat.first) << ", " << stat.second << ")\n";
    return 0;
}
