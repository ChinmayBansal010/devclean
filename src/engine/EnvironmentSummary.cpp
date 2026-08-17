#include "engine/EnvironmentSummary.hpp"

#include <algorithm>

#if defined(_WIN32)
constexpr const char* kPlatform = "windows";
#elif defined(__APPLE__)
constexpr const char* kPlatform = "macos";
#elif defined(__linux__)
constexpr const char* kPlatform = "linux";
#else
constexpr const char* kPlatform = "unknown";
#endif

EnvironmentSummary buildEnvironmentSummary(std::vector<EnvironmentTool> tools)
{
    EnvironmentSummary summary;
    summary.platform = kPlatform;
#if defined(__x86_64__) || defined(_M_X64)
    summary.architecture = "x86_64";
#elif defined(__aarch64__) || defined(_M_ARM64)
    summary.architecture = "arm64";
#else
    summary.architecture = "unknown";
#endif

    std::stable_sort(tools.begin(), tools.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.installed != rhs.installed)
            return lhs.installed > rhs.installed;
        return lhs.name < rhs.name;
    });

    for (const auto& tool : tools)
    {
        summary.installedCount += tool.installed ? 1 : 0;
        summary.activeCount += tool.active ? 1 : 0;
    }
    summary.tools = std::move(tools);
    return summary;
}
