#include "core/CacheInsights.hpp"
#include <cassert>
#include <chrono>
int main(){ScanSnapshot latest;latest.timestamp=std::chrono::system_clock::now();latest.totalBytes=150;ScanSnapshot previous;previous.timestamp=latest.timestamp-std::chrono::hours(24);previous.totalBytes=100;const auto i=buildCacheInsights({}, {latest,previous});assert(i.latestGrowthPercent==50.0);return 0;}
