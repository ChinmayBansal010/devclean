#pragma once

#include "scanner/CacheDefinition.hpp"

#include <filesystem>
#include <string>
#include <vector>

class PluginLoader
{
public:
    static PluginLoader& getInstance();

    std::vector<CacheDefinition> loadPlugins();
    void loadPluginsFromDirectory(const std::filesystem::path& pluginDir);
    std::vector<std::string> getLoadedPluginNames() const;
    std::vector<std::string> getFailedPlugins() const;

private:
    PluginLoader() = default;
    ~PluginLoader() = default;

    PluginLoader(const PluginLoader&) = delete;
    PluginLoader& operator=(const PluginLoader&) = delete;

    CacheDefinition parsePluginJson(const std::string& jsonContent,
                                    const std::filesystem::path& filePath);
    bool validatePluginDefinition(const CacheDefinition& cache);

    std::vector<CacheDefinition> loadedPlugins;
    std::vector<std::string> failedPlugins;
    std::vector<std::string> loadedPluginNames;
};
