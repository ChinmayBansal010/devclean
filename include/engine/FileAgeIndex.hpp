#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <vector>

struct FileAgeEntry
{
    std::filesystem::path path;
    uint64_t bytes = 0;
    std::chrono::seconds age{};
};

struct FileAgeSummary
{
    uint64_t totalBytes = 0;
    uint64_t staleBytes = 0;
    uint64_t fileCount = 0;
    uint64_t staleCount = 0;
};

std::vector<FileAgeEntry> indexFileAges(const std::filesystem::path& root,
                                        std::chrono::seconds staleAfter);
FileAgeSummary summarizeFileAges(const std::vector<FileAgeEntry>& entries,
                                 std::chrono::seconds staleAfter);
