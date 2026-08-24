#include "scanner/ScannerEngine.hpp"

#include "core/Application.hpp"
#include "core/ScanHistory.hpp"
#include "platform/Filesystem.hpp"
#include "platform/ToolDetector.hpp"
#include "scanner/CacheRegistry.hpp"
#include "scanner/PluginLoader.hpp"
#include "utils/StringUtils.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace {

bool matchesQuery(const ScanResult& result, const std::string& query)
{
    const std::string normalized = StringUtils::lower(query);
    if (normalized.empty()) return true;
    if (StringUtils::lower(result.name).find(normalized) != std::string::npos) return true;
    for (const auto& alias : result.aliases)
        if (StringUtils::lower(alias).find(normalized) != std::string::npos) return true;
    return false;
}

std::filesystem::path expandUserPath(std::string value)
{
    if (value.empty()) return {};
    if (value.size() > 1 && value[0] == '~' && (value[1] == '/' || value[1] == '\\'))
        if (const char* home = std::getenv("HOME")) value.replace(0, 1, home);
    return std::filesystem::path(value).lexically_normal();
}

std::filesystem::path environmentPathSuffix(const std::string& envVar)
{
    if (envVar == "CARGO_HOME") return "registry";
    if (envVar == "RUSTUP_HOME") return "toolchains";
    if (envVar == "GRADLE_USER_HOME") return "caches";
    if (envVar == "CONAN_USER_HOME") return "p";
    if (envVar == "VCPKG_ROOT") return "downloads";
    if (envVar == "XDG_CACHE_HOME") return "pacman";
    return {};
}

std::filesystem::path resolveConfiguredPath(const CacheDefinition& cache)
{
    for (const auto& envVar : cache.environmentVariables)
    {
        const char* rawValue = std::getenv(envVar.c_str());
        if (rawValue == nullptr || *rawValue == '\0') continue;
        auto candidate = expandUserPath(rawValue);
        if (!candidate.is_absolute())
        {
            std::error_code error;
            const auto absolute = std::filesystem::absolute(candidate, error);
            if (error) continue;
            candidate = absolute;
        }
        const auto suffix = environmentPathSuffix(envVar);
        return suffix.empty() ? candidate : candidate / suffix;
    }
#ifdef _WIN32
    if (!cache.windowsPath.empty()) return cache.windowsPath;
#else
    if (!cache.linuxPath.empty()) return cache.linuxPath;
#endif
    if (!cache.cachePaths.empty()) return cache.cachePaths.front();
    return {};
}

std::vector<CacheDefinition> mergeCaches(const std::vector<CacheDefinition>& base,
                                         const std::vector<CacheDefinition>& extra)
{
    std::vector<CacheDefinition> merged = base;
    auto alreadyPresent = [](const std::vector<CacheDefinition>& caches, const std::string& name) {
        return std::any_of(caches.begin(), caches.end(), [&](const CacheDefinition& cache) {
            if (StringUtils::equalsIgnoreCase(cache.name, name)) return true;
            return std::any_of(cache.aliases.begin(), cache.aliases.end(), [&](const std::string& alias) { return StringUtils::equalsIgnoreCase(alias, name); });
        });
    };
    for (const auto& cache : extra)
        if (!alreadyPresent(merged, cache.name)) merged.push_back(cache);
    return merged;
}

std::chrono::seconds computeAge(const std::filesystem::file_time_type& modified)
{
    if (modified == std::filesystem::file_time_type{}) return std::chrono::seconds{0};
    const auto delta = std::filesystem::file_time_type::clock::now() - modified;
    if (delta <= std::filesystem::file_time_type::duration::zero()) return std::chrono::seconds{0};
    return std::chrono::duration_cast<std::chrono::seconds>(delta);
}
}

std::vector<ScanResult> ScannerEngine::scan(const std::vector<std::string>& filters, const AppConfig& config)
{
    std::vector<ScanResult> results;
    auto caches = mergeCaches(CacheRegistry::getCaches(), config.customCaches);
    if (!filters.empty())
        caches.erase(std::remove_if(caches.begin(), caches.end(), [&](const CacheDefinition& cache) {
            return !std::any_of(filters.begin(), filters.end(), [&](const std::string& filter) {
                if (StringUtils::equalsIgnoreCase(cache.name, filter)) return true;
                return std::any_of(cache.aliases.begin(), cache.aliases.end(), [&](const std::string& alias) { return StringUtils::equalsIgnoreCase(alias, filter); });
            });
        }), caches.end());

    auto plugins = PluginLoader::getInstance().loadPlugins();
    if (!filters.empty())
    {
        std::vector<CacheDefinition> filteredPlugins;
        for (const auto& plugin : plugins)
            if (std::any_of(filters.begin(), filters.end(), [&](const std::string& filter) {
                    return StringUtils::equalsIgnoreCase(plugin.name, filter) || std::any_of(plugin.aliases.begin(), plugin.aliases.end(), [&](const std::string& alias) { return StringUtils::equalsIgnoreCase(alias, filter); });
                })) filteredPlugins.push_back(plugin);
        plugins = std::move(filteredPlugins);
    }
    caches = mergeCaches(caches, plugins);
    results.reserve(caches.size());
    const auto history = ScanHistory::getInstance().getHistory(2);
    const ScanSnapshot* previousSnapshot = history.size() > 1 ? &history[1] : nullptr;

    for (const auto& cache : caches)
    {
        if (Application::isInterrupted()) break;
        const auto path = resolveConfiguredPath(cache);
        ScanResult result;
        result.name = cache.name;
        result.aliases = cache.aliases;
        result.location = path;
        result.category = cache.category;
        result.enabled = cache.enabled;
        result.active = ToolDetector::getInstance().isInstalled(cache.name);
        if (!result.active)
            result.active = std::any_of(cache.aliases.begin(), cache.aliases.end(), [](const std::string& alias) { return ToolDetector::getInstance().isInstalled(alias); });
        result.warnings = ToolDetector::getInstance().getWarningsForCache(cache.name);
        if (std::any_of(config.disabledCaches.begin(), config.disabledCaches.end(), [&](const std::string& disabled) { return StringUtils::equalsIgnoreCase(disabled, result.name); }) ||
            std::any_of(config.ignoredCaches.begin(), config.ignoredCaches.end(), [&](const std::string& ignored) { return StringUtils::equalsIgnoreCase(ignored, result.name); }))
        {
            result.skipped = true;
            result.error = "ignored by configuration";
            results.push_back(std::move(result));
            continue;
        }
        if (!Filesystem::exists(path))
        {
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
            const auto previous = std::find_if(previousSnapshot->results.begin(), previousSnapshot->results.end(), [&](const ScanResult& past) { return StringUtils::equalsIgnoreCase(past.name, result.name); });
            if (previous != previousSnapshot->results.end())
                result.growthBytes = static_cast<int64_t>(result.bytes) - static_cast<int64_t>((*previous).bytes);
        }
        results.push_back(std::move(result));
    }

    std::vector<ScanResult> filtered;
    filtered.reserve(results.size());
    for (const auto& result)
        if (result.skipped || filters.empty() || std::any_of(filters.begin(), filters.end(), [&](const std::string& filter) { return matchesQuery(result, filter); })) filtered.push_back(result);
    return filtered;
}
