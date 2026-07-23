#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct ScanResult
{
    std::string name;
    std::vector<std::string> aliases;
    std::filesystem::path location;
    std::string category;
    uint64_t bytes = 0;
    uint64_t files = 0;
    uint64_t directories = 0;
    std::filesystem::file_time_type modified;
    bool found = false;
    bool enabled = true;
    bool skipped = false;
    std::string error;
};