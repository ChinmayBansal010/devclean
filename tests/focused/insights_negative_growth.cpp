#include "core/CacheInsights.hpp"
#include <cassert>
#include <filesystem>
int main(){ScanResult r;r.name="shrinking";r.found=true;r.bytes=100;r.growthBytes=-50;r.location=std::filesystem::temp_directory_path()/"devclean-shrinking";const auto i=buildCacheInsights({r},{});assert(i.cacheGrowth.front().second==-50);return 0;}
