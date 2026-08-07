#include "scanner/ScannerEngine.hpp"

#include "core/Application.hpp"
#include "core/ScanHistory.hpp"
#include "platform/Filesystem.hpp"
#include "platform/ToolDetector.hpp"
#include "scanner/CacheRegistry.hpp"
#include "scanner/PluginLoader.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <string>
#include <utility>
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

std::filesystem::path expandUserPath(std::string value)
{
    if (value.empty())
        return {};

    if (value.size() > 1 && value[0] == '~' && (value[1] == '/' || value[1] == '\\'))
    {
        if (const char* home = std::getenv("HOME"))
            value.replace(0, 1, home);
    }

    return std::filesystem::path(value).lexically_normal();
}

std::filesystem::path environmentPathSuffix(const std::string& envVar)
{
    if (envVar == "CARGO_HOME")
        return "registry";
    if (envVar == "RUSTUP_HOME")
        return "toolchains";
    if (envVar == "GRADLE_USER_HOME")
        return "caches";
    if (envVar == "CONAN_USER_HOME")
        return "p";
    if (envVar == "VCPKG_ROOT")
        return "downloads";
    if (envVar == "XDG_CACHE_HOME")
        return "pacman";

    return {};
}

std::filesystem::path resolveConfiguredPath(const CacheDefinition& cache)
{
    for (const auto& envVar : cache.environmentVariables)
    {
        const char* rawValue = std::getenv(envVar.c_str());
        if (rawValue == nullptr || *rawValue == '\0')
            continue;

        std::filesystem::path candidate = expandUserPath(rawValue);

        // Make relative paths absolute
        if (!candidate.is_absolute())
            candidate = std::filesystem::absolute(candidate);

        const auto suffix = environmentPathSuffix(envVar);
        return suffix.empty() ? candidate : candidate / suffix;
    }

#ifdef _WIN32
    if (!cache.windowsPath.empty())
        return cache.windowsPath;
#else
    if (!cache.linuxPath.empty())
        return cache.linuxPath;
#endif

    if (!cache.cachePaths.empty())
        return cache.cachePaths.front();

    return {};
}

std::vector<CacheDefinition> mergeCaches(const std::vector<CacheDefinition>& base,
                                         const std::vector<CacheDefinition>& extra)
{
    std::vector<CacheDefinition> merged = base;

    auto alreadyPresent = [](const std::vector<CacheDefinition>& caches, const std::string& name) {
        const std::string normalizedName = normalize(name);
        return std::any_of(caches.begin(), caches.end(), [&](const CacheDefinition& cache) {
            if (normalize(cache.name) == normalizedName)
                return true;
            return std::any_of(cache.aliases.begin(), cache.aliases.end(), [&](const std::string& alias) {
                return normalize(alias) == normalizedName;
            });
        });
    };

    for (const auto& cache : extra)
    {
        if (!alreadyPresent(merged, cache.name))
            merged.push_back(cache);
    }

    return merged;
}

std::chrono::seconds computeAge(const std::filesystem::file_time_type& modified)
{
    if (modified == std::filesystem::file_time_type{})
        return std::chrono::seconds{0};

    const auto now = std::filesystem::file_time_type::clock::now();
    const auto delta = now - modified;
    return std::chrono::duration_cast<std::chrono::seconds>(delta);
}

} // namespace

std::vector<ScanResult> ScannerEngine::scan(const std::vector<std::string>& filters, const AppConfig& config)
{
    std::vector<ScanResult> results;
    auto caches = mergeCaches(CacheRegistry::getCaches(), config.customCaches);
    auto plugins = PluginLoader::getInstance().loadPlugins();
    caches = mergeCaches(caches, plugins);
    results.reserve(caches.size());

    const auto history = ScanHistory::getInstance().getHistory(2);
    const ScanSnapshot* previousSnapshot = history.size() > 1 ? &history[1] : nullptr;

    for (const auto& cache : caches)
    {
        if (Application::isInterrupted())
            break;

        const auto path = resolveConfiguredPath(cache);

        ScanResult result;
        result.name = cache.name;
        result.aliases = cache.aliases;
        result.location = path;
        result.category = cache.category;
        result.enabled = cache.enabled;
        result.active = ToolDetector::getInstance().isInstalled(cache.name);
        if (!result.active)
        {
            result.active = std::any_of(cache.aliases.begin(), cache.aliases.end(), [](const std::string& alias) {
                return ToolDetector::getInstance().isInstalled(alias);
            });
        }
        result.warnings = ToolDetector::getInstance().getWarningsForCache(cache.name);

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
        result.age = computeAge(result.modified);
        result.fileTypeBytes = summary.fileTypeBytes;
        if (!summary.success)
        {
            result.found = false;
            result.skipped = true;
            result.error = summary.error.empty() ? "unreadable directory" : summary.error;
        }

        if (previousSnapshot != nullptr)
        {
            const auto previous = std::find_if(previousSnapshot->results.begin(), previousSnapshot->results.end(), [&](const ScanResult& past) {
                return normalize(past.name) == normalizedName;
            });
            if (previous != previousSnapshot->results.end())
            {
                result.growthBytes = static_cast<int64_t>(result.bytes) - static_cast<int64_t>((*previous).bytes);
            }
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
