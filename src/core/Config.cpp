#include "core/Config.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

std::string trim(const std::string& value)
{
    const std::size_t begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return {};
    const std::size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::vector<std::string> splitList(const std::string& value)
{
    std::vector<std::string> result;
    std::stringstream stream(value);
    std::string item;
    while (std::getline(stream, item, ','))
    {
        auto trimmed = trim(item);
        if (!trimmed.empty())
            result.push_back(trimmed);
    }
    return result;
}

std::vector<std::string> readStringArray(const json& source, const std::string& key)
{
    std::vector<std::string> values;
    if (!source.contains(key) || !source[key].is_array())
        return values;

    for (const auto& item : source[key])
    {
        if (item.is_string())
            values.push_back(item.get<std::string>());
    }
    return values;
}

std::vector<std::filesystem::path> readPathArray(const json& source, const std::string& key)
{
    std::vector<std::filesystem::path> values;
    if (!source.contains(key) || !source[key].is_array())
        return values;

    for (const auto& item : source[key])
    {
        if (item.is_string())
            values.emplace_back(item.get<std::string>());
    }
    return values;
}

void migrateLegacyConfig(json& root)
{
    if (!root.contains("disabledCaches") && root.contains("disabled") && root["disabled"].is_array())
        root["disabledCaches"] = root["disabled"];

    if (!root.contains("ignoredCaches") && root.contains("ignored") && root["ignored"].is_array())
        root["ignoredCaches"] = root["ignored"];

    if (!root.contains("defaultSort") && root.contains("default_sort"))
        root["defaultSort"] = root["default_sort"];

    if (!root.contains("defaultCategory") && root.contains("default_category"))
        root["defaultCategory"] = root["default_category"];

    if (!root.contains("schemaVersion"))
        root["schemaVersion"] = 3;

    if (!root.contains("version"))
        root["version"] = "3.0";
}

bool validateConfig(AppConfig& config)
{
    auto deduplicate = [](std::vector<std::string>& values) {
        std::vector<std::string> unique;
        for (const auto& value : values)
        {
            if (std::find(unique.begin(), unique.end(), value) == unique.end())
                unique.push_back(value);
        }
        values = std::move(unique);
    };

    deduplicate(config.disabledCaches);
    deduplicate(config.ignoredCaches);

    if (config.defaultSort.empty())
        config.defaultSort = "name";

    if (config.version.empty())
        config.version = "3.0";

    if (config.schemaVersion <= 0)
        config.schemaVersion = 3;

    return true;
}

std::filesystem::path getConfigPath()
{
    std::filesystem::path configPath;

#ifdef _WIN32
    if (const char* appData = std::getenv("APPDATA"))
        configPath = std::filesystem::path(appData) / "devclean" / "config.json";
#else
    if (const char* home = std::getenv("HOME"))
        configPath = std::filesystem::path(home) / ".config" / "devclean" / "config.json";
#endif

    return configPath;
}

} // namespace

AppConfig ConfigLoader::load()
{
    AppConfig config;

    std::filesystem::path configPath = getConfigPath();

    if (configPath.empty() || !std::filesystem::exists(configPath))
        return config;

    bool migrated = false;

    try {
        std::ifstream input(configPath);
        if (!input)
            return config;

        json j;
        input >> j;

        const std::string originalVersion = j.value("version", "");
        const int originalSchemaVersion = j.value("schemaVersion", 0);
        migrateLegacyConfig(j);
        migrated = originalVersion != j.value("version", "") || originalSchemaVersion < 3;

        if (j.contains("disabledCaches") && j["disabledCaches"].is_array()) {
            for (const auto& cache : j["disabledCaches"]) {
                if (cache.is_string())
                    config.disabledCaches.push_back(cache.get<std::string>());
            }
        }

        if (j.contains("ignoredCaches") && j["ignoredCaches"].is_array()) {
            for (const auto& cache : j["ignoredCaches"]) {
                if (cache.is_string())
                    config.ignoredCaches.push_back(cache.get<std::string>());
            }
        }

        config.defaultColor = j.value("defaultColor", "none");
        config.defaultSort = j.value("defaultSort", j.value("default_sort", "name"));
        config.defaultCategory = j.value("defaultCategory", j.value("default_category", ""));
        config.version = j.value("version", "3.0");

        if (j.contains("customCaches") && j["customCaches"].is_array()) {
            for (const auto& customJson : j["customCaches"]) {
                if (!customJson.is_object())
                    continue;

                CacheDefinition cache;
                cache.name = customJson.value("name", "");
                if (cache.name.empty())
                    continue;

                cache.category = customJson.value("category", "");
                cache.description = customJson.value("description", "");
                cache.enabled = customJson.value("enabled", true);
                cache.priority = customJson.value("priority", 0);

                for (const auto& alias : readStringArray(customJson, "aliases"))
                    cache.aliases.push_back(alias);
                for (const auto& variable : readStringArray(customJson, "environmentVariables"))
                    cache.environmentVariables.push_back(variable);
                for (const auto& support : readStringArray(customJson, "osSupport"))
                    cache.osSupport.push_back(support);
                for (const auto& path : readPathArray(customJson, "cachePaths"))
                    cache.cachePaths.push_back(path);

#ifdef _WIN32
                if (customJson.contains("windowsPath") && customJson["windowsPath"].is_string()) {
                    cache.windowsPath = std::filesystem::path(customJson["windowsPath"].get<std::string>());
                } else if (customJson.contains("path") && customJson["path"].is_string()) {
                    cache.windowsPath = std::filesystem::path(customJson["path"].get<std::string>());
                }
#else
                if (customJson.contains("linuxPath") && customJson["linuxPath"].is_string()) {
                    cache.linuxPath = std::filesystem::path(customJson["linuxPath"].get<std::string>());
                } else if (customJson.contains("path") && customJson["path"].is_string()) {
                    cache.linuxPath = std::filesystem::path(customJson["path"].get<std::string>());
                }
#endif

                config.customCaches.push_back(cache);
            }
        }

        config.schemaVersion = std::max(3, j.value("schemaVersion", 3));

        validateConfig(config);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to load config: " << e.what() << "\n";
    }

    if (migrated)
        save(config);

    return config;
}

void ConfigLoader::save(const AppConfig& config)
{
    std::filesystem::path configPath = getConfigPath();

    if (configPath.empty())
        return;

    std::filesystem::create_directories(configPath.parent_path());

    json j;
    j["schemaVersion"] = config.schemaVersion;
    j["disabledCaches"] = config.disabledCaches;
    j["ignoredCaches"] = config.ignoredCaches;
    j["defaultColor"] = config.defaultColor;
    j["defaultSort"] = config.defaultSort;
    j["defaultCategory"] = config.defaultCategory;
    j["version"] = config.version;

    json customCaches = json::array();
    for (const auto& cache : config.customCaches) {
        json cacheJson;
        cacheJson["name"] = cache.name;
        cacheJson["category"] = cache.category;
        cacheJson["description"] = cache.description;
        cacheJson["enabled"] = cache.enabled;
        cacheJson["priority"] = cache.priority;
        cacheJson["aliases"] = cache.aliases;
        cacheJson["environmentVariables"] = cache.environmentVariables;
        cacheJson["osSupport"] = cache.osSupport;
        cacheJson["cachePaths"] = json::array();
        for (const auto& path : cache.cachePaths)
            cacheJson["cachePaths"].push_back(path.string());

#ifdef _WIN32
        if (!cache.windowsPath.empty()) {
            cacheJson["windowsPath"] = cache.windowsPath.string();
        }
#else
        if (!cache.linuxPath.empty()) {
            cacheJson["linuxPath"] = cache.linuxPath.string();
        }
#endif

        customCaches.push_back(cacheJson);
    }
    j["customCaches"] = customCaches;

    std::ofstream output(configPath);
    output << j.dump(2);
}
