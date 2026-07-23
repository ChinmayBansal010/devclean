#include "commands/ScanCommand.hpp"

#include "core/Config.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>

namespace {

std::string normalize(const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
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

std::vector<ScanResult> applyFiltersAndSort(const std::vector<ScanResult>& input, const ParsedArgs& args)
{
    std::vector<ScanResult> results = input;

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

    std::stable_sort(results.begin(), results.end(), [&](const ScanResult& lhs, const ScanResult& rhs) {
        if (args.sort == "size")
            return lhs.bytes < rhs.bytes;
        if (args.sort == "modified")
            return lhs.modified < rhs.modified;
        return lhs.name < rhs.name;
    });

    if (args.reverse)
        std::reverse(results.begin(), results.end());

    return results;
}

void printSummary(const std::vector<ScanResult>& results)
{
    std::size_t found = 0;
    uint64_t totalBytes = 0;
    const ScanResult* largest = nullptr;
    std::map<std::string, std::size_t> categoryCounts;
    std::map<std::string, uint64_t> categoryBytes;

    for (const auto& result : results)
    {
        if (result.found)
        {
            ++found;
            totalBytes += result.bytes;
            if (largest == nullptr || result.bytes > largest->bytes)
                largest = &result;
            ++categoryCounts[result.category.empty() ? "misc" : result.category];
            categoryBytes[result.category.empty() ? "misc" : result.category] += result.bytes;
        }
    }

    std::cout << "\nSummary\n";
    std::cout << "-------\n";
    std::cout << "Total caches found: " << found << '\n';
    std::cout << "Total disk usage: " << Formatter::formatBytes(totalBytes) << '\n';
    if (largest != nullptr)
        std::cout << "Largest cache: " << largest->name << " (" << Formatter::formatBytes(largest->bytes) << ")\n";
    else
        std::cout << "Largest cache: none\n";

    std::string largestCategory;
    uint64_t largestCategoryBytes = 0;
    for (const auto& [category, bytes] : categoryBytes)
    {
        if (bytes > largestCategoryBytes)
        {
            largestCategoryBytes = bytes;
            largestCategory = category;
        }
    }
    std::cout << "Largest category: " << (largestCategory.empty() ? "none" : largestCategory) << '\n';
}

} // namespace

int ScanCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    if (args.category.empty() && !config.defaultCategory.empty())
        config.defaultCategory = config.defaultCategory;

    ScannerEngine engine;
    if (args.verbose)
        std::cout << "Scanning...\n";
    auto results = engine.scan(args.targets, config);
    auto filtered = applyFiltersAndSort(results, args);

    uint64_t total = 0;
    for (const auto& result : filtered)
        total += result.bytes;

    if (args.json)
    {
        nlohmann::json payload = nlohmann::json::object();
        payload["command"] = "scan";
        payload["total_bytes"] = total;

        nlohmann::json caches = nlohmann::json::array();
        for (const auto& result : filtered)
        {
            nlohmann::json entry = nlohmann::json::object();
            entry["name"] = result.name;
            entry["aliases"] = result.aliases;
            entry["path"] = result.location.string();
            entry["category"] = result.category;
            entry["found"] = result.found;
            entry["bytes"] = result.bytes;
            entry["files"] = result.files;
            entry["directories"] = result.directories;
            entry["error"] = result.error;
            caches.push_back(std::move(entry));
        }

        payload["caches"] = std::move(caches);
        std::cout << payload.dump(2) << '\n';
        return 0;
    }

    std::cout << "\nDetected caches\n";
    std::cout << "----------------------------------------\n";
    std::cout << std::left << std::setw(14) << "Name";
    std::cout << std::setw(12) << "Size";
    std::cout << std::setw(8) << "Files";
    std::cout << std::setw(12) << "Dirs";
    std::cout << std::setw(18) << "Modified";
    std::cout << std::setw(12) << "Category" << '\n';

    for (const auto& result : filtered)
    {
        std::cout << std::left << std::setw(14) << result.name;
        std::cout << std::setw(12) << Formatter::formatBytes(result.bytes);
        std::cout << std::setw(8) << result.files;
        std::cout << std::setw(12) << result.directories;
        std::cout << std::setw(18) << (result.found ? "present" : "missing");
        std::cout << std::setw(12) << (result.category.empty() ? "misc" : result.category) << '\n';
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Total: " << Formatter::formatBytes(total) << '\n';
    printSummary(filtered);

    if (args.verbose)
        std::cout << "Finished.\n";

    return 0;
}