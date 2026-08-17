#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct DuplicateGroup
{
    uint64_t size = 0;
    std::vector<std::filesystem::path> files;
    uint64_t reclaimableBytes = 0;
};

std::vector<DuplicateGroup> findDuplicateCandidates(const std::filesystem::path& root,
                                                     std::size_t minimumGroupSize = 2);
