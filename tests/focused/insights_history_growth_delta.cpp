#include "core/CacheInsights.hpp"
#include <cassert>
#include <chrono>
int main(){ScanSnapshot latest;latest.timestamp=std::chrono::system_clock::now();latest.totalBytes=300;ScanSnapshot previous;previous.timestamp=latest.timestamp-std::chrono::hours(24);previous.totalBytes=200;const auto i=buildCacheInsights({}, {latest,previous});assert(i.latestGrowthBytes==100);assert(i.averageGrowthBytes==100);return 0;}
