#pragma once

#include <filesystem>
#include <string>

struct CleanResult
{
    bool success = false;
    std::string error;
};

class CleanEngine
{
public:
    CleanResult removeDirectory(const std::filesystem::path& path);
};
