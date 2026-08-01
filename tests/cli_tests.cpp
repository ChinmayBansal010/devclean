#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "cleaner/CleanEngine.hpp"
#include "commands/ScanCommand.hpp"
#include "core/ArgumentParser.hpp"
#include "core/Config.hpp"
#include "platform/Filesystem.hpp"
#include "scanner/CacheRegistry.hpp"
#include "scanner/PluginLoader.hpp"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>

namespace {

void setEnvVar(const char* name, const std::string& value)
{
#ifdef _WIN32
    _putenv_s(name, value.c_str());

    if (std::string(name) == "HOME")
        _putenv_s("USERPROFILE", value.c_str());
#else
    setenv(name, value.c_str(), 1);
#endif
}

std::string getHomeEnv()
{
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    return home == nullptr ? std::string() : std::string(home);
}

} // namespace

int main()
{
    char* argv[] = {
        const_cast<char*>("devclean"),
        const_cast<char*>("clean"),
        const_cast<char*>("--force"),
        const_cast<char*>("--dry-run"),
        const_cast<char*>("python"),
        const_cast<char*>("cargo")
    };

    const ParsedArgs parsed = ArgumentParser::parse(6, argv);
    assert(parsed.command == "clean");
    assert(parsed.force);
    assert(parsed.dryRun);
    assert(parsed.targets.size() == 2);
    assert(parsed.targets[0] == "python");
    assert(parsed.targets[1] == "cargo");

    const auto pythonCaches = CacheRegistry::getMatchingCaches({"python"});
    assert(!pythonCaches.empty());
    assert(std::any_of(pythonCaches.begin(), pythonCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pip";
    }));

    const auto pipCache = std::find_if(pythonCaches.begin(), pythonCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pip";
    });
    assert(pipCache != pythonCaches.end());
#ifdef __APPLE__
    assert(pipCache->linuxPath == std::filesystem::path(getHomeEnv()) / "Library" / "Caches" / "pip");
#else
    assert(pipCache->linuxPath == std::filesystem::path(getHomeEnv()) / ".cache" / "pip");
#endif

    const auto rustCaches = CacheRegistry::getMatchingCaches({"rust"});
    assert(!rustCaches.empty());
    assert(std::any_of(rustCaches.begin(), rustCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "cargo";
    }));

    const auto pythonToolCaches = CacheRegistry::getMatchingCaches({"pipenv"});
    assert(!pythonToolCaches.empty());
    assert(std::any_of(pythonToolCaches.begin(), pythonToolCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pipenv";
    }));

    const auto nodeToolCaches = CacheRegistry::getMatchingCaches({"pnpm"});
    assert(!nodeToolCaches.empty());
    assert(std::any_of(nodeToolCaches.begin(), nodeToolCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pnpm";
    }));

    const auto buildToolCaches = CacheRegistry::getMatchingCaches({"ccache"});
    assert(!buildToolCaches.empty());
    assert(std::any_of(buildToolCaches.begin(), buildToolCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "ccache";
    }));

    const auto containerCaches = CacheRegistry::getMatchingCaches({"containers"});
    assert(std::any_of(containerCaches.begin(), containerCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "docker-builder" || cache.name == "docker-volumes" || cache.name == "podman";
    }));

    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        const std::filesystem::path configDir = std::filesystem::path(home) / ".config" / "devclean";
        std::filesystem::create_directories(configDir);
        const std::filesystem::path configPath = configDir / "config.json";
        std::ofstream output(configPath);
        output << R"({"disabled": ["cargo"], "ignored": ["npm"], "default_sort": "size", "schemaVersion": 1})";
        output.close();

        setEnvVar("HOME", home);
        const AppConfig migrated = ConfigLoader::load();
        assert(std::find(migrated.disabledCaches.begin(), migrated.disabledCaches.end(), "cargo") != migrated.disabledCaches.end());
        assert(std::find(migrated.ignoredCaches.begin(), migrated.ignoredCaches.end(), "npm") != migrated.ignoredCaches.end());
        assert(migrated.defaultSort == "size");
        assert(migrated.schemaVersion >= 3);
    }

    const auto tempRoot =
        std::filesystem::temp_directory_path() /
        ("devclean-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    setEnvVar("HOME", tempRoot.string());

    #ifdef _WIN32
    setEnvVar("USERPROFILE", tempRoot.string());
    #endif

    std::filesystem::path pluginDir;

    #ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    assert(appData != nullptr);

    pluginDir = std::filesystem::path(appData)
            / "devclean"
            / "plugins";
    #else
    pluginDir = tempRoot
            / ".config"
            / "devclean"
            / "plugins";
    #endif

    std::filesystem::create_directories(pluginDir);


    AppConfig customConfig;
    CacheDefinition customCache;
    customCache.name = "custom-cache";
    customCache.category = "build";
    customCache.description = "Custom plugin cache";
    customCache.enabled = true;
    customCache.priority = 7;
    customCache.aliases = {"custom"};
    customCache.environmentVariables = {"DEV_CLEAN_TEST"};
    customCache.osSupport = {"linux", "windows"};
    #ifdef _WIN32
    customCache.windowsPath = tempRoot / "custom-cache";
    #else
    customCache.linuxPath = tempRoot / "custom-cache";
    #endif
    customCache.cachePaths = {tempRoot / "custom-cache" / "." / "alt"};
    customConfig.customCaches.push_back(customCache);
    ConfigLoader::save(customConfig);

    const AppConfig reloadedConfig = ConfigLoader::load();
    assert(reloadedConfig.customCaches.size() == 1);
    assert(reloadedConfig.customCaches[0].description == "Custom plugin cache");
    assert(reloadedConfig.customCaches[0].priority == 7);
    assert(reloadedConfig.customCaches[0].environmentVariables.size() == 1);
    assert(reloadedConfig.customCaches[0].osSupport.size() == 2);
    assert(reloadedConfig.customCaches[0].aliases[0] == "custom");
    assert(!reloadedConfig.customCaches[0].cachePaths.empty());
    assert(reloadedConfig.customCaches[0].cachePaths[0] == (tempRoot / "custom-cache" / "alt"));

    const auto pipEnvCache = tempRoot / "pip-env-cache";
    std::filesystem::create_directories(pipEnvCache);
    setEnvVar("PIP_CACHE_DIR", pipEnvCache.string());

    ScanCommand scanCommand;
    ParsedArgs scanArgs;
    scanArgs.command = "scan";
    scanArgs.targets = {"pip"};
    scanArgs.json = true;
    std::ostringstream scanOutput;
    auto* originalBuffer = std::cout.rdbuf(scanOutput.rdbuf());
    scanCommand.execute(scanArgs);
    std::cout.rdbuf(originalBuffer);
    assert(scanOutput.str().find(pipEnvCache.string()) != std::string::npos);

    const auto pluginJson = pluginDir / "custom-plugin.json";

    std::ofstream pluginFile(pluginJson);
    assert(pluginFile.is_open());

    const auto pluginPath = tempRoot / "plugin-cache";
    const auto pluginAlt = pluginPath / "." / "alt";
        auto escapeJson = [](std::string s) {
        size_t pos = 0;
        while ((pos = s.find('\\', pos)) != std::string::npos) {
            s.replace(pos, 1, "\\\\");
            pos += 2;
        }
        return s;
    };

    const std::string pluginPathStr = escapeJson(pluginPath.string());
    const std::string pluginAltStr = escapeJson(pluginAlt.string());

    #ifdef _WIN32
    pluginFile
        << "{"
        << "\"name\":\"plugin-cache\","
        << "\"category\":\"build\","
        << "\"description\":\"Loaded from plugin\","
        << "\"enabled\":true,"
        << "\"priority\":5,"
        << "\"aliases\":[\"plugin\"],"
        << "\"environmentVariables\":[\"PLUGIN_CACHE\"],"
        << "\"osSupport\":[\"linux\",\"windows\"],"
        << "\"windowsPath\":\"" << pluginPathStr << "\","
        << "\"cachePaths\":[\"" << pluginAltStr << "\"]"
        << "}";
    #else
    pluginFile
        << "{"
        << "\"name\":\"plugin-cache\","
        << "\"category\":\"build\","
        << "\"description\":\"Loaded from plugin\","
        << "\"enabled\":true,"
        << "\"priority\":5,"
        << "\"aliases\":[\"plugin\"],"
        << "\"environmentVariables\":[\"PLUGIN_CACHE\"],"
        << "\"osSupport\":[\"linux\",\"windows\"],"
        << "\"linuxPath\":\"" << pluginPathStr << "\","
        << "\"cachePaths\":[\"" << pluginAltStr << "\"]"
        << "}";
    #endif

    pluginFile.close();

    assert(std::filesystem::exists(pluginJson));

    const auto loadedPlugins = PluginLoader::getInstance().loadPlugins();

    assert(!loadedPlugins.empty());
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [](const CacheDefinition& cache) {
        return cache.name == "plugin-cache";
    }));
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [](const CacheDefinition& cache) {
        return cache.name == "plugin-cache" && !cache.cachePaths.empty();
    }));
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [&](const CacheDefinition& cache) {
        return cache.name == "plugin-cache" && !cache.cachePaths.empty() && cache.cachePaths[0] == (pluginPath / "alt");
    }));

    std::ofstream invalidPluginFile(pluginDir / "invalid-plugin.json");
    invalidPluginFile << R"({"name":"invalid/plugin","category":"build","path":"../escape"})";
    invalidPluginFile.close();
    const auto loadedAfterInvalid = PluginLoader::getInstance().loadPlugins();
    assert(std::none_of(loadedAfterInvalid.begin(), loadedAfterInvalid.end(), [](const CacheDefinition& cache) {
        return cache.name == "invalid/plugin";
    }));

    assert(!Filesystem::isProtectedPath(tempRoot / "custom-cache"));
    const std::string homePath = getHomeEnv();
    if (!homePath.empty())
        assert(Filesystem::isProtectedPath(homePath));

    CleanEngine cleaner;
    const auto protectedRemoval = cleaner.removeDirectory(homePath.empty() ? std::filesystem::path("/") : std::filesystem::path(homePath));
    assert(!protectedRemoval.success);
    assert(!protectedRemoval.error.empty());

    const auto safeCacheDir = tempRoot / "safe-cache";
    std::filesystem::create_directories(safeCacheDir / "nested");
    std::ofstream(safeCacheDir / "nested" / "artifact.txt") << "cache";
    const auto removed = cleaner.removeDirectory(safeCacheDir);
    assert(removed.success);
    assert(!std::filesystem::exists(safeCacheDir));
    std::filesystem::remove_all(tempRoot);
    std::cout << "devclean CLI tests passed" << std::endl;
    return 0;
}
