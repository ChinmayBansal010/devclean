#include "core/CacheInsights.hpp"
#include <cassert>
int main(){ScanResult a;a.name="slow";a.found=true;a.bytes=10;a.growthBytes=2;ScanResult b;b.name="fast";b.found=true;b.bytes=20;b.growthBytes=8;const auto i=buildCacheInsights({a,b},{});assert(i.cacheGrowth.front().first=="fast");assert(i.cacheGrowth.front().second==8);return 0;}
