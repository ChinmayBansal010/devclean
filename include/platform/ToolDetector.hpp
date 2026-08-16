#pragma once

#include <string>
#include <unordered_set>
#include <vector>

struct ToolStatus
{
    std::string name;
    bool installed = false;
    bool running = false;
};

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
    std::vector<ToolStatus> getEnvironmentReport() const;

private:
    ToolDetector() = default;
    ~ToolDetector() = default;

    ToolDetector(const ToolDetector&) = delete;
    ToolDetector& operator=(const ToolDetector&) = delete;
};
