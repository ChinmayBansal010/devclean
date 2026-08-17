#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct CleanupManifestEntry
{
    std::string cache;
    std::string path;
    uint64_t bytes = 0;
    bool completed = false;
};

struct CleanupManifest
{
    std::string createdAt;
    std::string operationId;
    std::vector<CleanupManifestEntry> entries;
};

std::string createCleanupOperationId();
std::string serializeCleanupManifest(const CleanupManifest& manifest);
