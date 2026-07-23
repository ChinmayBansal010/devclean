#include "commands/CleanCommand.hpp"

#include "cleaner/CleanEngine.hpp"
#include "core/Config.hpp"
#include "platform/ToolDetector.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

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

std::vector<ScanResult> applyFilters(const std::vector<ScanResult>& input, const ParsedArgs& args)
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

    return results;
}

bool isInteractiveTerminal()
{
    return std::cin.good();
}

std::vector<ScanResult> selectCandidates(const std::vector<ScanResult>& candidates)
{
    std::vector<ScanResult> selected = candidates;
    if (candidates.size() <= 1 || !isInteractiveTerminal())
        return selected;

    std::cout << "Select caches to delete:\n";
    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        std::cout << "[ ] " << candidates[i].name << "\n";
    }
    std::cout << "Enter names separated by spaces, or press Enter to delete all: ";

    std::string input;
    std::getline(std::cin, input);
    if (input.empty())
        return selected;

    std::vector<std::string> requested;
    std::istringstream stream(input);
    std::string token;
    while (stream >> token)
        requested.push_back(normalize(token));

    if (requested.empty())
        return {};

    selected.clear();
    for (const auto& candidate : candidates)
    {
        const std::string normalizedName = normalize(candidate.name);
        if (std::any_of(requested.begin(), requested.end(), [&](const std::string& item) {
                return normalizedName == item || std::any_of(candidate.aliases.begin(), candidate.aliases.end(), [&](const std::string& alias) {
                    return normalize(alias) == item;
                });
            }))
        {
            selected.push_back(candidate);
        }
    }

    return selected;
}

} // namespace

int CleanCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ScannerEngine scanner;
    CleanEngine cleaner;

    auto results = scanner.scan(args.targets, config);
    auto candidates = applyFilters(results, args);
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [](const ScanResult& result) {
        return !result.found;
    }), candidates.end());

    if (candidates.empty())
    {
        std::cout << "No cache directories were found to remove.\n";
        return 0;
    }

    if (args.json)
    {
        nlohmann::json payload = nlohmann::json::object();
        payload["command"] = "clean";
        payload["dry_run"] = args.dryRun;
        payload["force"] = args.force;
        payload["targets"] = args.targets;
        payload["caches"] = nlohmann::json::array();
        for (const auto& candidate : candidates)
        {
            nlohmann::json entry = nlohmann::json::object();
            entry["name"] = candidate.name;
            entry["path"] = candidate.location.string();
            entry["bytes"] = candidate.bytes;
            payload["caches"].push_back(std::move(entry));
        }
        std::cout << payload.dump(2) << '\n';
        return 0;
    }

    auto selected = selectCandidates(candidates);
    if (selected.empty())
    {
        std::cout << "No caches selected for deletion.\n";
        return 0;
    }

    std::cout << "Caches that can be removed:\n\n";
    for (const auto& candidate : selected)
    {
        std::cout << "  " << candidate.name << " (" << Formatter::formatBytes(candidate.bytes) << ")\n";
        const auto warnings = ToolDetector::getInstance().getWarningsForCache(candidate.name);
        for (const auto& warning : warnings)
            std::cout << "    warning: " << warning << "\n";
    }

    if (args.dryRun)
    {
        std::cout << "\nDry run: no directories were removed.\n";
        return 0;
    }

    if (!args.force)
    {
        std::cout << "\nProceed? [y/N]: ";
        char response = 'n';
        std::cin >> response;

        if (response != 'y' && response != 'Y')
        {
            std::cout << "Cancelled.\n";
            return 0;
        }
    }

    int exitCode = 0;
    for (const auto& candidate : selected)
    {
        if (args.verbose)
            std::cout << "Deleting... " << candidate.name << '\n';
        if (cleaner.removeDirectory(candidate.location))
        {
            std::cout << "[OK]   " << candidate.name << '\n';
        }
        else
        {
            std::cout << "[SKIP] " << candidate.name << '\n';
            if (exitCode == 0)
                exitCode = 1;
        }
    }

    if (args.verbose)
        std::cout << "Finished.\n";

    return exitCode;
}