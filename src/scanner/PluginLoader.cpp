#include "scanner/PluginLoader.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PluginLoader& PluginLoader::getInstance()
{
    static PluginLoader instance;
    return instance;
}

std::vector<CacheDefinition> PluginLoader::loadPlugins()
{
    std::filesystem::path pluginDir;

#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        pluginDir = std::filesystem::path(appData) / "devclean" / "plugins";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        pluginDir = std::filesystem::path(home) / ".config" / "devclean" /
                    "plugins";
    }
#endif

    if (!pluginDir.empty() && std::filesystem::exists(pluginDir)) {
        loadPluginsFromDirectory(pluginDir);
    }

    return loadedPlugins;
}

void PluginLoader::loadPluginsFromDirectory(
    const std::filesystem::path& pluginDir)
{
    if (!std::filesystem::exists(pluginDir)) {
        return;
    }

    try {
        for (const auto& entry :
             std::filesystem::directory_iterator(pluginDir)) {
            if (entry.path().extension() == ".json") {
                try {
                    std::ifstream file(entry.path());
                    std::string content((std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());

                    CacheDefinition cache =
                        parsePluginJson(content, entry.path());
                    if (validatePluginDefinition(cache)) {
                        loadedPlugins.push_back(cache);
                        loadedPluginNames.push_back(cache.name);
                    } else {
                        failedPlugins.push_back(entry.path().filename()
                                                   .string());
                    }
                } catch (const std::exception& e) {
                    failedPlugins.push_back(entry.path().filename().string());
                    std::cerr << "Warning: Failed to load plugin "
                              << entry.path().filename().string() << ": "
                              << e.what() << "\n";
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Error reading plugin directory: " << e.what()
                  << "\n";
    }
}

CacheDefinition PluginLoader::parsePluginJson(
    const std::string& jsonContent, const std::filesystem::path& filePath)
{
    json j = json::parse(jsonContent);

    CacheDefinition cache;
    cache.name = j.value("name", "");
    cache.category = j.value("category", "");
    cache.description = j.value("description", "");
    cache.enabled = j.value("enabled", true);
    cache.priority = j.value("priority", 0);

    if (j.contains("aliases") && j["aliases"].is_array()) {
        for (const auto& alias : j["aliases"]) {
            cache.aliases.push_back(alias.get<std::string>());
        }
    }

    if (j.contains("environmentVariables") && j["environmentVariables"].is_array()) {
        for (const auto& variable : j["environmentVariables"]) {
            cache.environmentVariables.push_back(variable.get<std::string>());
        }
    }

    if (j.contains("osSupport") && j["osSupport"].is_array()) {
        for (const auto& support : j["osSupport"]) {
            cache.osSupport.push_back(support.get<std::string>());
        }
    }

#ifdef _WIN32
    if (j.contains("windowsPath")) {
        cache.windowsPath =
            std::filesystem::path(j["windowsPath"].get<std::string>());
    }
    if (j.contains("path")) {
        cache.windowsPath =
            std::filesystem::path(j["path"].get<std::string>());
    }
#else
    if (j.contains("linuxPath")) {
        cache.linuxPath =
            std::filesystem::path(j["linuxPath"].get<std::string>());
    }
    if (j.contains("path")) {
        cache.linuxPath =
            std::filesystem::path(j["path"].get<std::string>());
    }
#endif

    return cache;
}

bool PluginLoader::validatePluginDefinition(const CacheDefinition& cache)
{
    if (cache.name.empty()) {
        return false;
    }

#ifdef _WIN32
    if (cache.windowsPath.empty()) {
        return false;
    }
#else
    if (cache.linuxPath.empty()) {
        return false;
    }
#endif

    return true;
}

std::vector<std::string> PluginLoader::getLoadedPluginNames() const
{
    return loadedPluginNames;
}

std::vector<std::string> PluginLoader::getFailedPlugins() const
{
    return failedPlugins;
}
