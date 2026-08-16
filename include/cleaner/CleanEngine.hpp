#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

struct CleanResult
{
    bool success = false;
    std::string error;
    uint64_t bytesRemoved = 0;
    uint64_t filesRemoved = 0;
};

class CleanEngine
{
public:
    CleanResult removeDirectory(const std::filesystem::path& path);
    CleanResult removeStaleFiles(const std::filesystem::path& path, uint64_t staleSeconds, uint64_t targetBytes = 0);
};
