#pragma once

#include "scanner/ScanResult.hpp"

#include <cstdint>
#include <string>
#include <vector>

struct ScanChange
{
    std::string name;
    int64_t byteDelta = 0;
    int64_t fileDelta = 0;
    bool appeared = false;
    bool disappeared = false;
};

struct ScanDiff
{
    uint64_t beforeBytes = 0;
    uint64_t afterBytes = 0;
    int64_t byteDelta = 0;
    std::vector<ScanChange> changes;
};

ScanDiff compareScans(const std::vector<ScanResult>& before,
                      const std::vector<ScanResult>& after);
