#include "scanner/ScannerEngine.hpp"

#include "platform/Filesystem.hpp"
#include "scanner/CacheRegistry.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <string>
#include <utility>

namespace {

std::string normalize(const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

bool matchesQuery(const ScanResult& result, const std::string& query)
{
    const std::string normalized = normalize(query);
    if (normalized.empty())
        return true;

    if (normalize(result.name).find(normalized) != std::string::npos)
        return true;

    for (const auto& alias : result.aliases)
    {
        if (normalize(alias).find(normalized) != std::string::npos)
            return true;
    }

    return false;
}

std::string normalizeCategory(const std::string& value)
{
    std::string normalized = normalize(value);
    if (normalized == "node")
        return "javascript";
    if (normalized == "package-managers" || normalized == "package managers")
        return "package managers";
    if (normalized == "cpp")
        return "cpp";
    return normalized;
}

} // namespace

std::vector<ScanResult> ScannerEngine::scan(const std::vector<std::string>& filters, const AppConfig& config)
{
    std::vector<ScanResult> results;
    auto caches = CacheRegistry::getMatchingCaches(filters);
    results.reserve(caches.size());

    for (const auto& cache : caches)
    {
#ifdef _WIN32
        const auto path = cache.windowsPath;
#else
        const auto path = cache.linuxPath;
#endif

        ScanResult result;
        result.name = cache.name;
        result.aliases = cache.aliases;
        result.location = path;
        result.category = cache.category;
        result.enabled = cache.enabled;

        const std::string normalizedName = normalize(result.name);
        if (std::any_of(config.disabledCaches.begin(), config.disabledCaches.end(), [&](const std::string& disabled) {
                return normalize(disabled) == normalizedName;
            }) || std::any_of(config.ignoredCaches.begin(), config.ignoredCaches.end(), [&](const std::string& ignored) {
                return normalize(ignored) == normalizedName;
            }))
        {
            result.skipped = true;
            result.error = "ignored by configuration";
            results.push_back(std::move(result));
            continue;
        }

        if (!Filesystem::exists(path))
        {
            result.found = false;
            result.error = "missing directory";
            results.push_back(std::move(result));
            continue;
        }

        const auto summary = Filesystem::inspectDirectory(path);
        result.found = summary.exists;
        result.bytes = summary.bytes;
        result.files = summary.files;
        result.directories = summary.directories;
        result.modified = Filesystem::lastModified(path);
        if (!summary.success)
        {
            result.found = false;
            result.skipped = true;
            result.error = summary.error.empty() ? "unreadable directory" : summary.error;
        }

        results.push_back(std::move(result));
    }

    std::vector<ScanResult> filtered;
    filtered.reserve(results.size());
    for (const auto& result : results)
    {
        if (result.skipped)
        {
            filtered.push_back(result);
            continue;
        }

        if (filters.empty() || std::any_of(filters.begin(), filters.end(), [&](const std::string& filter) {
                return matchesQuery(result, filter);
            }))
        {
            filtered.push_back(result);
        }
    }

    return filtered;
}