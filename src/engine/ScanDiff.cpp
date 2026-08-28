#include "engine/ScanDiff.hpp"

#include <algorithm>
#include <limits>
#include <map>

namespace {

int64_t toSignedBytes(uint64_t bytes)
{
    constexpr uint64_t maxSigned = static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
    return bytes > maxSigned ? std::numeric_limits<int64_t>::max() : static_cast<int64_t>(bytes);
}

} // namespace

ScanDiff compareScans(const std::vector<ScanResult>& before,
                      const std::vector<ScanResult>& after)
{
    std::map<std::string, ScanResult> oldValues;
    std::map<std::string, ScanResult> newValues;

    for (const auto& result : before)
        if (result.found)
            oldValues[result.name] = result;
    for (const auto& result : after)
        if (result.found)
            newValues[result.name] = result;

    ScanDiff diff;
    for (const auto& [name, result] : oldValues)
        diff.beforeBytes += result.bytes;
    for (const auto& [name, result] : newValues)
        diff.afterBytes += result.bytes;

    diff.byteDelta = toSignedBytes(diff.afterBytes) - toSignedBytes(diff.beforeBytes);

    auto oldIt = oldValues.begin();
    auto newIt = newValues.begin();
    while (oldIt != oldValues.end() || newIt != newValues.end())
    {
        if (newIt == newValues.end() || (oldIt != oldValues.end() && oldIt->first < newIt->first))
        {
            ScanChange change;
            change.name = oldIt->first;
            change.byteDelta = -toSignedBytes(oldIt->second.bytes);
            change.fileDelta = -static_cast<int64_t>(oldIt->second.files);
            change.disappeared = true;
            diff.changes.push_back(std::move(change));
            ++oldIt;
            continue;
        }

        if (oldIt == oldValues.end() || newIt->first < oldIt->first)
        {
            ScanChange change;
            change.name = newIt->first;
            change.byteDelta = toSignedBytes(newIt->second.bytes);
            change.fileDelta = static_cast<int64_t>(newIt->second.files);
            change.appeared = true;
            diff.changes.push_back(std::move(change));
            ++newIt;
            continue;
        }

        ScanChange change;
        change.name = newIt->first;
        change.byteDelta = toSignedBytes(newIt->second.bytes) - toSignedBytes(oldIt->second.bytes);
        change.fileDelta = static_cast<int64_t>(newIt->second.files) - static_cast<int64_t>(oldIt->second.files);
        if (change.byteDelta != 0 || change.fileDelta != 0)
            diff.changes.push_back(std::move(change));
        ++oldIt;
        ++newIt;
    }

    std::stable_sort(diff.changes.begin(), diff.changes.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.byteDelta != rhs.byteDelta)
            return lhs.byteDelta > rhs.byteDelta;
        return lhs.name < rhs.name;
    });
    return diff;
}
