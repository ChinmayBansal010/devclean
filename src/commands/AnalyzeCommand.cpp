#include "commands/AnalyzeCommand.hpp"

#include "core/Config.hpp"
#include "core/ScanHistory.hpp"
#include "platform/ToolDetector.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>

namespace {

struct AnalysisData
{
    std::vector<ScanResult> results;
    std::vector<ScanResult> foundCaches;
    std::vector<std::pair<std::string, uint64_t>> largestFolders;
    std::map<std::string, uint64_t> fileTypeBytes;
    std::map<std::string, uint64_t> categoryBytes;
    std::vector<std::pair<std::string, int64_t>> growth;
    std::vector<std::pair<std::string, uint64_t>> ages;
    std::vector<std::string> safeCaches;
    std::vector<std::string> largeCaches;
    std::vector<std::string> unusedCaches;
    std::vector<std::string> oldCaches;
};

std::string normalize(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string escapeCsv(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size() + 4);
    for (char ch : value)
    {
        if (ch == '"')
            escaped += '"';
        escaped += ch;
    }
    if (escaped.find_first_of(",\"\n") != std::string::npos)
        return '"' + escaped + '"';
    return escaped;
}

int64_t ageInDays(const ScanResult& result)
{
    if (result.age.count() <= 0)
        return 0;
    return std::chrono::duration_cast<std::chrono::hours>(result.age).count() / 24;
}

AnalysisData buildAnalysis(const std::vector<ScanResult>& results)
{
    AnalysisData data;
    data.results = results;

    ToolDetector& detector = ToolDetector::getInstance();
    for (const auto& result : results)
    {
        if (!result.found)
        {
            data.unusedCaches.push_back(result.name);
            continue;
        }

        data.foundCaches.push_back(result);
        data.largestFolders.emplace_back(result.name, result.bytes);
        data.ages.emplace_back(result.name, static_cast<uint64_t>(std::max<int64_t>(0, ageInDays(result))));
        data.categoryBytes[result.category.empty() ? "misc" : result.category] += result.bytes;

        for (const auto& [type, bytes] : result.fileTypeBytes)
            data.fileTypeBytes[type] += bytes;

        if (result.growthBytes != 0)
            data.growth.emplace_back(result.name, result.growthBytes);

        const auto warnings = detector.getWarningsForCache(result.name);
        const bool isSafe = warnings.empty() && !result.active;
        if (isSafe)
            data.safeCaches.push_back(result.name);

        if (result.bytes >= 250ULL * 1024ULL * 1024ULL)
            data.largeCaches.push_back(result.name);

        if (ageInDays(result) >= 30)
            data.oldCaches.push_back(result.name);
    }

    std::sort(data.largestFolders.begin(), data.largestFolders.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    std::sort(data.growth.begin(), data.growth.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    std::sort(data.ages.begin(), data.ages.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    return data;
}

void printMarkdownReport(const AnalysisData& data)
{
    std::cout << "# devclean analysis\n\n";

    std::cout << "## Largest folders\n";
    for (const auto& [name, bytes] : data.largestFolders)
        std::cout << "- " << name << " - " << Formatter::formatBytes(bytes) << "\n";

    std::cout << "\n## Largest file types\n";
    for (const auto& [type, bytes] : data.fileTypeBytes)
        std::cout << "- " << type << " - " << Formatter::formatBytes(bytes) << "\n";

    std::cout << "\n## Cache age\n";
    for (const auto& [name, days] : data.ages)
        std::cout << "- " << name << " - " << days << " days\n";

    std::cout << "\n## Growth\n";
    for (const auto& [name, growth] : data.growth)
    {
        const std::string sign = growth >= 0 ? "+" : "-";
        std::cout << "- " << name << " - " << sign << Formatter::formatBytes(static_cast<uint64_t>(growth >= 0 ? growth : -growth)) << "\n";
    }

    std::cout << "\n## Recommendations\n";
    if (!data.safeCaches.empty())
    {
        std::cout << "- Safe caches: ";
        for (std::size_t i = 0; i < data.safeCaches.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << data.safeCaches[i];
        }
        std::cout << "\n";
    }
    if (!data.largeCaches.empty())
    {
        std::cout << "- Large caches: ";
        for (std::size_t i = 0; i < data.largeCaches.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << data.largeCaches[i];
        }
        std::cout << "\n";
    }
    if (!data.unusedCaches.empty())
    {
        std::cout << "- Unused caches: ";
        for (std::size_t i = 0; i < data.unusedCaches.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << data.unusedCaches[i];
        }
        std::cout << "\n";
    }
    if (!data.oldCaches.empty())
    {
        std::cout << "- Old caches: ";
        for (std::size_t i = 0; i < data.oldCaches.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << data.oldCaches[i];
        }
        std::cout << "\n";
    }
}

void printHtmlReport(const AnalysisData& data)
{
    std::cout << "<html><body><h1>devclean analysis</h1>";
    std::cout << "<h2>Largest folders</h2><ul>";
    for (const auto& [name, bytes] : data.largestFolders)
        std::cout << "<li>" << name << " - " << Formatter::formatBytes(bytes) << "</li>";
    std::cout << "</ul><h2>Largest file types</h2><ul>";
    for (const auto& [type, bytes] : data.fileTypeBytes)
        std::cout << "<li>" << type << " - " << Formatter::formatBytes(bytes) << "</li>";
    std::cout << "</ul><h2>Recommendations</h2><ul>";
    for (const auto& name : data.largeCaches)
        std::cout << "<li>Large cache: " << name << "</li>";
    for (const auto& name : data.safeCaches)
        std::cout << "<li>Safe cache: " << name << "</li>";
    for (const auto& name : data.oldCaches)
        std::cout << "<li>Old cache: " << name << "</li>";
    std::cout << "</ul></body></html>\n";
}

void printCsvReport(const AnalysisData& data)
{
    std::cout << "section,name,value\n";
    for (const auto& [name, bytes] : data.largestFolders)
        std::cout << "largest_folder," << escapeCsv(name) << ',' << bytes << '\n';
    for (const auto& [type, bytes] : data.fileTypeBytes)
        std::cout << "file_type," << escapeCsv(type) << ',' << bytes << '\n';
    for (const auto& [name, days] : data.ages)
        std::cout << "age," << escapeCsv(name) << ',' << days << '\n';
}

void printJsonReport(const AnalysisData& data)
{
    nlohmann::json payload = nlohmann::json::object();

    payload["largest_folders"] = nlohmann::json::array();
    for (const auto& [name, bytes] : data.largestFolders)
    {
        payload["largest_folders"].push_back({{"name", name}, {"bytes", bytes}});
    }

    payload["largest_file_types"] = nlohmann::json::array();
    for (const auto& [type, bytes] : data.fileTypeBytes)
    {
        payload["largest_file_types"].push_back({{"name", type}, {"bytes", bytes}});
    }

    payload["cache_age"] = nlohmann::json::array();
    for (const auto& [name, days] : data.ages)
    {
        payload["cache_age"].push_back({{"name", name}, {"days", days}});
    }

    payload["growth"] = nlohmann::json::array();
    for (const auto& [name, growth] : data.growth)
    {
        payload["growth"].push_back({{"name", name}, {"bytes", growth}});
    }

    payload["recommendations"] = nlohmann::json::object();
    payload["recommendations"]["safe"] = data.safeCaches;
    payload["recommendations"]["large"] = data.largeCaches;
    payload["recommendations"]["unused"] = data.unusedCaches;
    payload["recommendations"]["old"] = data.oldCaches;

    std::cout << payload.dump(2) << '\n';
}

} // namespace

int AnalyzeCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ScannerEngine engine;
    auto results = engine.scan(args.targets, config);
    ScanHistory::getInstance().recordScan(results);
    auto analysis = buildAnalysis(results);

    if (!args.reportFormat.empty())
    {
        const std::string format = normalize(args.reportFormat);
        if (format == "markdown")
            printMarkdownReport(analysis);
        else if (format == "html")
            printHtmlReport(analysis);
        else if (format == "csv")
            printCsvReport(analysis);
        else if (format == "json")
            printJsonReport(analysis);
        else
            std::cout << "Unsupported report format: " << args.reportFormat << '\n';
        return 0;
    }

    std::cout << "\nAnalysis\n";
    std::cout << "--------\n";

    std::cout << "Largest folders:\n";
    for (std::size_t i = 0; i < std::min<std::size_t>(analysis.largestFolders.size(), 8); ++i)
        std::cout << "- " << analysis.largestFolders[i].first << " : " << Formatter::formatBytes(analysis.largestFolders[i].second) << '\n';

    std::cout << "\nLargest file types:\n";
    for (const auto& [type, bytes] : analysis.fileTypeBytes)
        std::cout << "- " << type << " : " << Formatter::formatBytes(bytes) << '\n';

    std::cout << "\nCache age:\n";
    for (std::size_t i = 0; i < std::min<std::size_t>(analysis.ages.size(), 8); ++i)
        std::cout << "- " << analysis.ages[i].first << " : " << analysis.ages[i].second << " days\n";

    std::cout << "\nGrowth:\n";
    if (analysis.growth.empty())
    {
        std::cout << "- no historical growth data available yet\n";
    }
    else
    {
        for (const auto& [name, growth] : analysis.growth)
        {
            const std::string sign = growth >= 0 ? "+" : "-";
            std::cout << "- " << name << " : " << sign << Formatter::formatBytes(static_cast<uint64_t>(growth >= 0 ? growth : -growth)) << '\n';
        }
    }

    std::cout << "\nRecommendations:\n";
    if (analysis.safeCaches.empty() && analysis.largeCaches.empty() && analysis.unusedCaches.empty() && analysis.oldCaches.empty())
    {
        std::cout << "- no recommendations at this time\n";
    }
    else
    {
        if (!analysis.safeCaches.empty())
            std::cout << "- safe caches: " << analysis.safeCaches.front() << (analysis.safeCaches.size() > 1 ? " ..." : "") << '\n';
        if (!analysis.largeCaches.empty())
            std::cout << "- large caches: " << analysis.largeCaches.front() << (analysis.largeCaches.size() > 1 ? " ..." : "") << '\n';
        if (!analysis.unusedCaches.empty())
            std::cout << "- unused caches: " << analysis.unusedCaches.front() << (analysis.unusedCaches.size() > 1 ? " ..." : "") << '\n';
        if (!analysis.oldCaches.empty())
            std::cout << "- old caches: " << analysis.oldCaches.front() << (analysis.oldCaches.size() > 1 ? " ..." : "") << '\n';
    }

    return 0;
}
