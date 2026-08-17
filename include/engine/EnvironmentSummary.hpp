#pragma once

#include <string>
#include <vector>

struct EnvironmentTool
{
    std::string name;
    std::string version;
    bool installed = false;
    bool active = false;
};

struct EnvironmentSummary
{
    std::string platform;
    std::string architecture;
    std::vector<EnvironmentTool> tools;
    std::size_t installedCount = 0;
    std::size_t activeCount = 0;
};

EnvironmentSummary buildEnvironmentSummary(std::vector<EnvironmentTool> tools);
