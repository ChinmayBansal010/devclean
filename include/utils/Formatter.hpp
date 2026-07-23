#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

class Formatter
{
public:
    static std::string formatBytes(uint64_t bytes);
    static std::string formatPath(const std::filesystem::path& path);
};
