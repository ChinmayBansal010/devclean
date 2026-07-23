#pragma once

#include <filesystem>

class CleanEngine
{
public:
    bool removeDirectory(const std::filesystem::path& path);
};