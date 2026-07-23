#pragma once

#include <string>
#include <unordered_set>
#include <vector>

class ToolDetector
{
public:
    static ToolDetector& getInstance();

    bool isInstalled(const std::string& toolName);
    bool isRunning(const std::string& processName);
    bool isDockerDaemonRunning();
    bool isIdeRunning();
    bool hasActivePackageManager();
    std::vector<std::string> getWarningsForCache(const std::string& cacheName);
    std::unordered_set<std::string> detectInstalledTools();

private:
    ToolDetector() = default;
    ~ToolDetector() = default;

    ToolDetector(const ToolDetector&) = delete;
    ToolDetector& operator=(const ToolDetector&) = delete;

    bool checkToolInstalled(const std::string& toolName);
    std::unordered_set<std::string> detectedTools;
    bool initialized = false;
};
