#pragma once

#include "scanner/ScanResult.hpp"

#include <chrono>
#include <string>
#include <vector>

struct ScanSnapshot
{
    std::chrono::system_clock::time_point timestamp;
    std::vector<ScanResult> results;
    uint64_t totalBytes = 0;
    uint64_t totalFiles = 0;
    uint64_t totalDirectories = 0;

    std::string getTimestampString() const;
};

class ScanHistory
{
public:
    static ScanHistory& getInstance();

    void recordScan(const std::vector<ScanResult>& results);
    std::vector<ScanSnapshot> getHistory(size_t limit = 10);
    ScanSnapshot getLatestScan();
    ScanSnapshot getScanAt(size_t index);

    void clearHistory();

    std::vector<std::string> getDifferences(size_t fromIndex, size_t toIndex);

private:
    ScanHistory();
    ~ScanHistory() = default;

    ScanHistory(const ScanHistory&) = delete;
    ScanHistory& operator=(const ScanHistory&) = delete;

    void loadFromDisk();
    void saveToDisk();
    std::string getHistoryFilePath();

    std::vector<ScanSnapshot> snapshots;
    static constexpr size_t MAX_SNAPSHOTS = 100;
};
