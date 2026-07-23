#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

class Filesystem
{
public:
    struct DirectorySummary
    {
        bool exists = false;
        bool success = true;
        uint64_t bytes = 0;
        uint64_t files = 0;
        uint64_t directories = 0;
        std::string error;
    };

    static bool exists(const std::filesystem::path& path);
    static DirectorySummary inspectDirectory(const std::filesystem::path& path);
    static uint64_t directorySize(const std::filesystem::path& path);
    static bool removeDirectory(const std::filesystem::path& path);
    static std::filesystem::file_time_type lastModified(const std::filesystem::path& path);
    static uint64_t fileCount(const std::filesystem::path& path);
    static uint64_t directoryCount(const std::filesystem::path& path);
};