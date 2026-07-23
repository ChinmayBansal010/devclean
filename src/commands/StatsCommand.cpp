#include "commands/StatsCommand.hpp"

#include "core/Config.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

int StatsCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ScannerEngine scanner;
    auto results = scanner.scan(args.targets, config);

    uint64_t totalBytes = 0;
    uint64_t totalFiles = 0;
    uint64_t totalDirectories = 0;
    std::size_t foundCount = 0;
    std::map<std::string, uint64_t> categoryBytes;
    std::map<std::string, std::size_t> categoryCounts;
    std::vector<std::pair<std::string, uint64_t>> largestCaches;

    for (const auto& result : results)
    {
        totalBytes += result.bytes;
        totalFiles += result.files;
        totalDirectories += result.directories;
        if (result.found)
        {
            ++foundCount;
            const std::string category = result.category.empty() ? "misc" : result.category;
            categoryBytes[category] += result.bytes;
            ++categoryCounts[category];
            largestCaches.emplace_back(result.name, result.bytes);
        }
    }

    std::stable_sort(largestCaches.begin(), largestCaches.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    if (args.json)
    {
        nlohmann::json payload = nlohmann::json::object();
        payload["command"] = "stats";
        payload["cache_count"] = results.size();
        payload["found_count"] = foundCount;
        payload["total_bytes"] = totalBytes;
        payload["total_files"] = totalFiles;
        payload["total_directories"] = totalDirectories;
        payload["largest_caches"] = nlohmann::json::array();
        for (std::size_t i = 0; i < std::min<std::size_t>(largestCaches.size(), 5); ++i)
        {
            nlohmann::json entry = nlohmann::json::object();
            entry["name"] = largestCaches[i].first;
            entry["bytes"] = largestCaches[i].second;
            payload["largest_caches"].push_back(std::move(entry));
        }
        payload["largest_categories"] = nlohmann::json::array();
        for (const auto& [category, bytes] : categoryBytes)
        {
            nlohmann::json entry = nlohmann::json::object();
            entry["name"] = category;
            entry["bytes"] = bytes;
            entry["count"] = categoryCounts[category];
            payload["largest_categories"].push_back(std::move(entry));
        }
        std::cout << payload.dump(2) << '\n';
        return 0;
    }

    std::cout << "Statistics\n";
    std::cout << "----------\n";
    std::cout << "Caches scanned: " << results.size() << '\n';
    std::cout << "Caches found: " << foundCount << '\n';
    std::cout << "Total size: " << Formatter::formatBytes(totalBytes) << '\n';
    std::cout << "Total files: " << totalFiles << '\n';
    std::cout << "Total directories: " << totalDirectories << '\n';
    std::cout << "\nLargest caches:\n";
    for (std::size_t i = 0; i < std::min<std::size_t>(largestCaches.size(), 5); ++i)
        std::cout << "  - " << largestCaches[i].first << " (" << Formatter::formatBytes(largestCaches[i].second) << ")\n";
    std::cout << "\nLargest categories:\n";
    for (const auto& [category, bytes] : categoryBytes)
        std::cout << "  - " << category << " (" << Formatter::formatBytes(bytes) << ", " << categoryCounts[category] << ")\n";
    return 0;
}