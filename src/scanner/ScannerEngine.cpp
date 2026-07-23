#include "scanner/ScannerEngine.hpp"

#include "core/ScanHistory.hpp"
#include "platform/Filesystem.hpp"
#include "platform/ToolDetector.hpp"
#include "scanner/CacheRegistry.hpp"
#include "scanner/PluginLoader.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <map>
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

std::filesystem::path pickPath(const CacheDefinition& cache)
{
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

std::map<std::string, uint64_t> collectFileTypes(const std::filesystem::path& root)
{
    std::map<std::string, uint64_t> fileTypes;
    if (root.empty())
        return fileTypes;

    std::error_code ec;
    for (std::filesystem::recursive_directory_iterator it(root, std::filesystem::directory_options::skip_permission_denied, ec), end;
         it != end;
         it.increment(ec))
    {
        if (ec)
            break;

        const auto& entry = *it;
        std::error_code entryEc;
        if (!entry.is_regular_file(entryEc) || entryEc)
            continue;

        const auto ext = entry.path().extension().string();
        std::error_code sizeEc;
        const auto fileSize = entry.file_size(sizeEc);
        if (sizeEc)
            continue;

        fileTypes[ext.empty() ? "<no extension>" : normalize(ext)] += fileSize;
    }
    return fileTypes;
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
        const auto path = pickPath(cache);

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
        result.fileTypeBytes = collectFileTypes(path);
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
