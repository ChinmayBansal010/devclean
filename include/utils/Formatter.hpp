#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

class Formatter
{
public:
    static std::string formatBytes(uint64_t bytes);
    static std::string formatSignedBytes(int64_t bytes);
    static std::string formatCount(uint64_t count);
    static std::string formatPath(const std::filesystem::path& path);
};
