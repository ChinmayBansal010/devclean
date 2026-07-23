#pragma once

#include <cstdint>
#include <chrono>
#include <filesystem>
#include <map>
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
    int64_t growthBytes = 0;
    std::chrono::seconds age{};
    std::filesystem::file_time_type modified;
    bool found = false;
    bool enabled = true;
    bool skipped = false;
    bool active = false;
    std::string error;
    std::map<std::string, uint64_t> fileTypeBytes;
    std::vector<std::string> warnings;
};
