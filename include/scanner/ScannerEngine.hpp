#pragma once

#include "core/Config.hpp"
#include "scanner/ScanResult.hpp"

#include <string>
#include <vector>

class ScannerEngine
{
public:
    std::vector<ScanResult> scan(
        const std::vector<std::string>& filters = {},
        const AppConfig& config = {}
    );
};