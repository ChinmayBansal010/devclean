#pragma once

#include "scanner/ScanResult.hpp"

#include <string>
#include <vector>

struct ReportRow
{
    std::string name;
    std::string category;
    std::string path;
    uint64_t bytes = 0;
    uint64_t files = 0;
    int64_t growthBytes = 0;
    bool active = false;
};

struct ReportModel
{
    std::string title;
    std::string version;
    std::vector<ReportRow> rows;
};

ReportModel buildReportModel(const std::vector<ScanResult>& results,
                             const std::string& version);
