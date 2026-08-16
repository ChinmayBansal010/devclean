#include "commands/CleanCommand.hpp"

#include "cleaner/CleanEngine.hpp"
#include "core/Config.hpp"
#include "platform/Filesystem.hpp"
#include "platform/ToolDetector.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

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

    if (args.activeOnly)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [](const ScanResult& result) {
            return !result.active;
        }), results.end());
    }

    if (args.safe)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [](const ScanResult& result) {
            return result.active || !result.warnings.empty() || Filesystem::isProtectedPath(result.location);
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

    if (args.sort == "size")
    {
        std::stable_sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.bytes < rhs.bytes;
        });
    }
    else if (args.sort == "modified")
    {
        std::stable_sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.modified < rhs.modified;
        });
    }
    else
    {
        std::stable_sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.name < rhs.name;
        });
    }

    if (args.reverse)
        std::reverse(results.begin(), results.end());

    return results;
}

std::vector<ScanResult> applyTarget(const std::vector<ScanResult>& candidates, uint64_t targetBytes)
{
    if (targetBytes == 0)
        return candidates;

    std::vector<ScanResult> ordered = candidates;
    std::stable_sort(ordered.begin(), ordered.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.bytes < rhs.bytes;
    });

    std::vector<ScanResult> selected;
    uint64_t planned = 0;
    for (const auto& candidate : ordered)
    {
        if (candidate.bytes == 0 || candidate.bytes > targetBytes - planned)
            continue;
        selected.push_back(candidate);
        planned += candidate.bytes;
        if (planned >= targetBytes)
            break;
    }
    return selected;
}

bool isInteractiveTerminal()
{
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0 && _isatty(_fileno(stdout)) != 0;
#else
    return isatty(fileno(stdin)) != 0 && isatty(fileno(stdout)) != 0;
#endif
}

std::vector<ScanResult> selectCandidates(const std::vector<ScanResult>& candidates)
{
    std::vector<ScanResult> selected = candidates;
    if (candidates.size() <= 1 || !isInteractiveTerminal())
        return selected;

    std::cout << "Select caches to delete:\n";
    for (const auto& candidate : candidates)
        std::cout << "[ ] " << candidate.name << "\n";
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

}

int CleanCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ParsedArgs effectiveArgs = args;

    if (effectiveArgs.category.empty() && !config.defaultCategory.empty())
        effectiveArgs.category = config.defaultCategory;

    ScannerEngine scanner;
    CleanEngine cleaner;
    auto results = scanner.scan(effectiveArgs.targets, config);
    auto candidates = applyFilters(results, effectiveArgs);
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [](const ScanResult& result) {
        return !result.found;
    }), candidates.end());

    if (effectiveArgs.targetSizeBytes > 0)
        candidates = applyTarget(candidates, effectiveArgs.targetSizeBytes);

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
        payload["safe"] = args.safe;
        payload["stale_seconds"] = args.staleSeconds;
        payload["target_size_bytes"] = args.targetSizeBytes;
        payload["active_only"] = args.activeOnly;
        payload["min_size_bytes"] = args.minSizeBytes;
        payload["max_size_bytes"] = args.maxSizeBytes;
        payload["targets"] = effectiveArgs.targets;
        payload["caches"] = nlohmann::json::array();
        for (const auto& candidate : candidates)
        {
            payload["caches"].push_back({
                {"name", candidate.name},
                {"path", candidate.location.string()},
                {"bytes", candidate.bytes},
                {"active", candidate.active},
                {"warnings", candidate.warnings}
            });
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

    std::cout << "Caches selected for cleanup:\n\n";
    for (const auto& candidate : selected)
    {
        std::cout << "  " << candidate.name << " (" << Formatter::formatBytes(candidate.bytes) << ")\n";
        const auto warnings = ToolDetector::getInstance().getWarningsForCache(candidate.name);
        for (const auto& warning : warnings)
            std::cout << "    warning: " << warning << "\n";
    }

    if (args.safe)
        std::cout << "\nSafe mode: only inactive, unprotected caches without active-tool warnings are eligible.\n";

    if (args.staleSeconds > 0)
        std::cout << "Stale mode: only files older than " << args.staleSeconds << " seconds will be removed.\n";

    if (args.dryRun)
    {
        std::cout << "\nDry run: no files or directories were removed.\n";
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
    uint64_t totalBytesRemoved = 0;
    uint64_t totalFilesRemoved = 0;
    uint64_t remainingTarget = args.targetSizeBytes;

    for (const auto& candidate : selected)
    {
        if (args.verbose)
            std::cout << "Cleaning... " << candidate.name << '\n';

        CleanResult cleanResult;
        if (args.staleSeconds > 0)
            cleanResult = cleaner.removeStaleFiles(candidate.location, args.staleSeconds, remainingTarget);
        else
            cleanResult = cleaner.removeDirectory(candidate.location);

        totalBytesRemoved += cleanResult.bytesRemoved;
        totalFilesRemoved += cleanResult.filesRemoved;

        if (args.targetSizeBytes > 0)
        {
            if (cleanResult.bytesRemoved >= remainingTarget)
                remainingTarget = 0;
            else
                remainingTarget -= cleanResult.bytesRemoved;
        }

        if (cleanResult.success)
            std::cout << "[OK]   " << candidate.name << "\n";
        else
        {
            std::cout << "[SKIP] " << candidate.name << "\n";
            if (!cleanResult.error.empty())
                std::cout << "       " << cleanResult.error << '\n';
            if (exitCode == 0)
                exitCode = 1;
        }

        if (args.targetSizeBytes > 0 && remainingTarget == 0)
            break;
    }

    if (args.staleSeconds > 0)
        std::cout << "Removed " << totalFilesRemoved << " stale files (" << Formatter::formatBytes(totalBytesRemoved) << ").\n";

    if (args.verbose)
        std::cout << "Finished.\n";

    return exitCode;
}
