#pragma once

#include <filesystem>
#include <string>

struct PathSafetyResult
{
    bool safe = false;
    bool exists = false;
    bool symlink = false;
    bool protectedLocation = false;
    std::string reason;
};

PathSafetyResult inspectPathSafety(const std::filesystem::path& path);
bool isPathInside(const std::filesystem::path& child, const std::filesystem::path& parent);
