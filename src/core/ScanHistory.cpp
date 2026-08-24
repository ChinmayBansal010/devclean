#include "core/ScanHistory.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <utility>

using json = nlohmann::json;

namespace {
uint64_t saturatingAdd(uint64_t lhs, uint64_t rhs)
{
    if (rhs > std::numeric_limits<uint64_t>::max() - lhs)
        return std::numeric_limits<uint64_t>::max();
    return lhs + rhs;
}
}

std::string ScanSnapshot::getTimestampString() const
{
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

ScanHistory& ScanHistory::getInstance()
{
    static ScanHistory instance;
    return instance;
}

ScanHistory::ScanHistory() { loadFromDisk(); }

std::string ScanHistory::getHistoryFilePath()
{
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData) return std::string(appData) + "\\devclean\\history.json";
#else
    const char* home = std::getenv("HOME");
    if (home) return std::string(home) + "/.config/devclean/history.json";
#endif
    return "";
}

void ScanHistory::recordScan(const std::vector<ScanResult>& results)
{
    ScanSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.results = results;
    for (const auto& result : results)
    {
        snapshot.totalBytes = saturatingAdd(snapshot.totalBytes, result.bytes);
        snapshot.totalFiles = saturatingAdd(snapshot.totalFiles, result.files);
        snapshot.totalDirectories = saturatingAdd(snapshot.totalDirectories, result.directories);
    }
    snapshots.insert(snapshots.begin(), snapshot);
    if (snapshots.size() > MAX_SNAPSHOTS)
        snapshots.erase(snapshots.begin() + MAX_SNAPSHOTS, snapshots.end());
    saveToDisk();
}

std::vector<ScanSnapshot> ScanHistory::getHistory(size_t limit)
{
    if (snapshots.empty()) return {};
    const size_t count = std::min(limit, snapshots.size());
    return std::vector<ScanSnapshot>(snapshots.begin(), snapshots.begin() + count);
}

ScanSnapshot ScanHistory::getLatestScan() { return snapshots.empty() ? ScanSnapshot() : snapshots.front(); }
ScanSnapshot ScanHistory::getScanAt(size_t index) { return index >= snapshots.size() ? ScanSnapshot() : snapshots[index]; }

void ScanHistory::clearHistory()
{
    snapshots.clear();
    saveToDisk();
}

std::vector<std::string> ScanHistory::getDifferences(size_t fromIndex, size_t toIndex)
{
    std::vector<std::string> diffs;
    if (fromIndex >= snapshots.size() || toIndex >= snapshots.size()) return diffs;
    const auto& fromSnapshot = snapshots[fromIndex];
    const auto& toSnapshot = snapshots[toIndex];
    auto fromMap = [](const std::vector<ScanResult>& results) {
        std::map<std::string, const ScanResult*> m;
        for (const auto& r : results) m[r.name] = &r;
        return m;
    };
    const auto from = fromMap(fromSnapshot.results);
    const auto to = fromMap(toSnapshot.results);
    for (const auto& [name, toResult] : to)
    {
        if (from.find(name) == from.end()) diffs.push_back("NEW: " + name);
        else if (toResult->bytes > from.at(name)->bytes)
            diffs.push_back("GROWTH: " + name + " (+" + std::to_string(toResult->bytes - from.at(name)->bytes) + " bytes)");
        else if (toResult->bytes < from.at(name)->bytes)
            diffs.push_back("SHRINK: " + name + " (-" + std::to_string(from.at(name)->bytes - toResult->bytes) + " bytes)");
    }
    for (const auto& [name, fromResult] : from)
        if (to.find(name) == to.end()) diffs.push_back("REMOVED: " + name);
    return diffs;
}

void ScanHistory::loadFromDisk()
{
    const std::string filePath = getHistoryFilePath();
    if (filePath.empty()) return;
    std::ifstream file(filePath);
    if (!file.good()) return;
    try
    {
        json j;
        file >> j;
        if (!j.is_array()) return;
        for (const auto& snapshot : j)
        {
            ScanSnapshot ss;
            ss.timestamp = std::chrono::system_clock::from_time_t(snapshot.value("timestamp", 0));
            ss.totalBytes = snapshot.value("totalBytes", 0ULL);
            ss.totalFiles = snapshot.value("totalFiles", 0ULL);
            ss.totalDirectories = snapshot.value("totalDirectories", 0ULL);
            if (snapshot.contains("results") && snapshot["results"].is_array())
                for (const auto& result : snapshot["results"])
                {
                    ScanResult sr;
                    sr.name = result.value("name", "");
                    sr.category = result.value("category", "");
                    sr.bytes = result.value("bytes", 0ULL);
                    sr.files = result.value("files", 0ULL);
                    sr.directories = result.value("directories", 0ULL);
                    ss.results.push_back(std::move(sr));
                }
            snapshots.push_back(std::move(ss));
        }
        if (snapshots.size() > MAX_SNAPSHOTS) snapshots.resize(MAX_SNAPSHOTS);
    }
    catch (const std::exception&) {}
}

void ScanHistory::saveToDisk()
{
    const std::string filePath = getHistoryFilePath();
    if (filePath.empty()) return;
    const auto separator = filePath.rfind(
#ifdef _WIN32
        '\\'
#else
        '/'
#endif
    );
    if (separator != std::string::npos) std::filesystem::create_directories(filePath.substr(0, separator));
    try
    {
        json j = json::array();
        for (const auto& snapshot : snapshots)
        {
            json ss;
            ss["timestamp"] = std::chrono::system_clock::to_time_t(snapshot.timestamp);
            ss["totalBytes"] = snapshot.totalBytes;
            ss["totalFiles"] = snapshot.totalFiles;
            ss["totalDirectories"] = snapshot.totalDirectories;
            ss["results"] = json::array();
            for (const auto& result : snapshot.results)
                ss["results"].push_back({{"name", result.name}, {"category", result.category}, {"bytes", result.bytes}, {"files", result.files}, {"directories", result.directories}});
            j.push_back(std::move(ss));
        }
        std::ofstream file(filePath, std::ios::trunc);
        if (file) file << j.dump(2);
    }
    catch (const std::exception&) {}
}
