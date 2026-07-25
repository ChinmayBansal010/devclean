#pragma once

#include "scanner/CacheDefinition.hpp"

#include <string>
#include <vector>

class CacheRegistry
{
public:
    static std::vector<CacheDefinition> getCaches();
    static std::vector<CacheDefinition> getMatchingCaches(
        const std::vector<std::string>& filters = {}
    );
};
