#include "core/Config.hpp"

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

    std::ifstream input(configPath);
    if (!input)
        return config;

    try {
        json j;
        input >> j;

        auto readStringArray = [&](const std::string& key) {
            std::vector<std::string> values;
            if (j.contains(key) && j[key].is_array()) {
                for (const auto& item : j[key]) {
                    if (item.is_string())
                        values.push_back(item.get<std::string>());
                }
            }
            return values;
        };

        if (j.contains("disabledCaches") && j["disabledCaches"].is_array()) {
            for (const auto& cache : j["disabledCaches"]) {
                config.disabledCaches.push_back(cache.get<std::string>());
            }
        } else if (j.contains("disabled") && j["disabled"].is_array()) {
            for (const auto& cache : j["disabled"]) {
                config.disabledCaches.push_back(cache.get<std::string>());
            }
        }

        if (j.contains("ignoredCaches") && j["ignoredCaches"].is_array()) {
            for (const auto& cache : j["ignoredCaches"]) {
                config.ignoredCaches.push_back(cache.get<std::string>());
            }
        } else if (j.contains("ignored") && j["ignored"].is_array()) {
            for (const auto& cache : j["ignored"]) {
                config.ignoredCaches.push_back(cache.get<std::string>());
            }
        }

        config.defaultColor = j.value("defaultColor", "none");
        config.defaultSort = j.value("defaultSort", j.value("default_sort", "name"));
        config.defaultCategory = j.value("defaultCategory", j.value("default_category", ""));
        config.version = j.value("version", "1.1");

        if (j.contains("customCaches") && j["customCaches"].is_array()) {
            for (const auto& customJson : j["customCaches"]) {
                CacheDefinition cache;
                cache.name = customJson.value("name", "");
                cache.category = customJson.value("category", "");
                cache.enabled = customJson.value("enabled", true);

                if (customJson.contains("aliases") &&
                    customJson["aliases"].is_array()) {
                    for (const auto& alias : customJson["aliases"]) {
                        cache.aliases.push_back(alias.get<std::string>());
                    }
                }

#ifdef _WIN32
                if (customJson.contains("windowsPath")) {
                    cache.windowsPath = std::filesystem::path(
                        customJson["windowsPath"].get<std::string>());
                }
#else
                if (customJson.contains("linuxPath")) {
                    cache.linuxPath = std::filesystem::path(
                        customJson["linuxPath"].get<std::string>());
                }
#endif

                config.customCaches.push_back(cache);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to load config: " << e.what() << "\n";
    }

    return config;
}

void ConfigLoader::save(const AppConfig& config)
{
    std::filesystem::path configPath = getConfigPath();

    if (configPath.empty())
        return;

    std::filesystem::create_directories(configPath.parent_path());

    json j;
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
        cacheJson["enabled"] = cache.enabled;
        cacheJson["aliases"] = cache.aliases;

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
