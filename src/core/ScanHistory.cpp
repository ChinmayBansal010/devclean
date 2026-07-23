#include "core/ScanHistory.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <map>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

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

ScanHistory::ScanHistory()
{
    loadFromDisk();
}

std::string ScanHistory::getHistoryFilePath()
{
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        std::string path = std::string(appData) + "\\devclean\\history.json";
        return path;
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        std::string path = std::string(home) + "/.config/devclean/history.json";
        return path;
    }
#endif
    return "";
}

void ScanHistory::recordScan(const std::vector<ScanResult>& results)
{
    ScanSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.results = results;

    for (const auto& result : results) {
        snapshot.totalBytes += result.bytes;
        snapshot.totalFiles += result.files;
        snapshot.totalDirectories += result.directories;
    }

    snapshots.insert(snapshots.begin(), snapshot);

    if (snapshots.size() > MAX_SNAPSHOTS) {
        snapshots.erase(snapshots.begin() + MAX_SNAPSHOTS, snapshots.end());
    }

    saveToDisk();
}

std::vector<ScanSnapshot> ScanHistory::getHistory(size_t limit)
{
    if (snapshots.empty()) {
        return {};
    }

    size_t count = std::min(limit, snapshots.size());
    return std::vector<ScanSnapshot>(snapshots.begin(),
                                     snapshots.begin() + count);
}

ScanSnapshot ScanHistory::getLatestScan()
{
    if (snapshots.empty()) {
        return ScanSnapshot();
    }
    return snapshots.front();
}

ScanSnapshot ScanHistory::getScanAt(size_t index)
{
    if (index >= snapshots.size()) {
        return ScanSnapshot();
    }
    return snapshots[index];
}

void ScanHistory::clearHistory()
{
    snapshots.clear();
    saveToDisk();
}

std::vector<std::string> ScanHistory::getDifferences(size_t fromIndex,
                                                     size_t toIndex)
{
    std::vector<std::string> diffs;

    if (fromIndex >= snapshots.size() || toIndex >= snapshots.size()) {
        return diffs;
    }

    const auto& fromSnapshot = snapshots[fromIndex];
    const auto& toSnapshot = snapshots[toIndex];

    auto fromMap = [](const std::vector<ScanResult>& results) {
        std::map<std::string, const ScanResult*> m;
        for (const auto& r : results) {
            m[r.name] = &r;
        }
        return m;
    };

    auto from = fromMap(fromSnapshot.results);
    auto to = fromMap(toSnapshot.results);

    for (const auto& [name, toResult] : to) {
        if (from.find(name) == from.end()) {
            diffs.push_back("NEW: " + name);
        } else {
            auto fromResult = from[name];
            if (toResult->bytes > fromResult->bytes) {
                uint64_t diff = toResult->bytes - fromResult->bytes;
                diffs.push_back("GROWTH: " + name + " (+" +
                                std::to_string(diff) + " bytes)");
            } else if (toResult->bytes < fromResult->bytes) {
                uint64_t diff = fromResult->bytes - toResult->bytes;
                diffs.push_back("SHRINK: " + name + " (-" +
                                std::to_string(diff) + " bytes)");
            }
        }
    }

    for (const auto& [name, fromResult] : from) {
        if (to.find(name) == to.end()) {
            diffs.push_back("REMOVED: " + name);
        }
    }

    return diffs;
}

void ScanHistory::loadFromDisk()
{
    std::string filePath = getHistoryFilePath();
    if (filePath.empty()) {
        return;
    }

    std::ifstream file(filePath);
    if (!file.good()) {
        return;
    }

    try {
        json j;
        file >> j;

        if (j.is_array()) {
            for (const auto& snapshot : j) {
                ScanSnapshot ss;

                auto timestamp = snapshot.value("timestamp", 0);
                ss.timestamp = std::chrono::system_clock::from_time_t(timestamp);
                ss.totalBytes = snapshot.value("totalBytes", 0UL);
                ss.totalFiles = snapshot.value("totalFiles", 0UL);
                ss.totalDirectories = snapshot.value("totalDirectories", 0UL);

                if (snapshot.contains("results") &&
                    snapshot["results"].is_array()) {
                    for (const auto& result : snapshot["results"]) {
                        ScanResult sr;
                        sr.name = result.value("name", "");
                        sr.category = result.value("category", "");
                        sr.bytes = result.value("bytes", 0UL);
                        sr.files = result.value("files", 0UL);
                        sr.directories = result.value("directories", 0UL);
                        ss.results.push_back(sr);
                    }
                }

                snapshots.push_back(ss);
            }
        }
    } catch (const std::exception&) {
    }
}

void ScanHistory::saveToDisk()
{
    std::string filePath = getHistoryFilePath();
    if (filePath.empty()) {
        return;
    }

#ifdef _WIN32
    size_t pos = filePath.rfind('\\');
#else
    size_t pos = filePath.rfind('/');
#endif

    if (pos != std::string::npos) {
        std::string dir = filePath.substr(0, pos);
        std::filesystem::create_directories(dir);
    }

    try {
        json j = json::array();

        for (const auto& snapshot : snapshots) {
            json ss;
            ss["timestamp"] =
                std::chrono::system_clock::to_time_t(snapshot.timestamp);
            ss["totalBytes"] = snapshot.totalBytes;
            ss["totalFiles"] = snapshot.totalFiles;
            ss["totalDirectories"] = snapshot.totalDirectories;

            json results = json::array();
            for (const auto& result : snapshot.results) {
                json r;
                r["name"] = result.name;
                r["category"] = result.category;
                r["bytes"] = result.bytes;
                r["files"] = result.files;
                r["directories"] = result.directories;
                results.push_back(r);
            }
            ss["results"] = results;

            j.push_back(ss);
        }

        std::ofstream file(filePath);
        file << j.dump(2);
    } catch (const std::exception&) {
    }
}
