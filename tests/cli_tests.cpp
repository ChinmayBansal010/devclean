#include "core/ArgumentParser.hpp"
#include "core/Config.hpp"
#include "scanner/CacheRegistry.hpp"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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

    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        const std::filesystem::path configDir = std::filesystem::path(home) / ".config" / "devclean";
        std::filesystem::create_directories(configDir);
        const std::filesystem::path configPath = configDir / "config.json";
        std::ofstream output(configPath);
        output << R"({"disabled": ["cargo"], "ignored": ["npm"], "default_sort": "size"})";
        output.close();

        setenv("HOME", home, 1);
        const AppConfig migrated = ConfigLoader::load();
        assert(std::find(migrated.disabledCaches.begin(), migrated.disabledCaches.end(), "cargo") != migrated.disabledCaches.end());
        assert(std::find(migrated.ignoredCaches.begin(), migrated.ignoredCaches.end(), "npm") != migrated.ignoredCaches.end());
        assert(migrated.defaultSort == "size");
    }

    std::cout << "devclean CLI tests passed" << std::endl;
    return 0;
}
