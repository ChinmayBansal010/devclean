#pragma once

#include <cstdint>
#include <filesystem>

struct SizeEstimate
{
    uint64_t bytes = 0;
    uint64_t files = 0;
    uint64_t directories = 0;
    bool complete = true;
};

SizeEstimate estimateDirectorySize(const std::filesystem::path& root);
