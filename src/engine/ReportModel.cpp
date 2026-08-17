#include "engine/ReportModel.hpp"

#include <algorithm>

ReportModel buildReportModel(const std::vector<ScanResult>& results,
                             const std::string& version)
{
    ReportModel model;
    model.title = "devclean cache report";
    model.version = version;
    model.rows.reserve(results.size());

    for (const auto& result : results)
    {
        if (!result.found)
            continue;
        ReportRow row;
        row.name = result.name;
        row.category = result.category;
        row.path = result.location.string();
        row.bytes = result.bytes;
        row.files = result.files;
        row.growthBytes = result.growthBytes;
        row.active = result.active;
        model.rows.push_back(std::move(row));
    }

    std::stable_sort(model.rows.begin(), model.rows.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.bytes != rhs.bytes)
            return lhs.bytes > rhs.bytes;
        return lhs.name < rhs.name;
    });
    return model;
}
