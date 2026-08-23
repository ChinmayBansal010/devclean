#include "core/CacheInsights.hpp"
#include <cassert>
#include <filesystem>
int main(){ScanResult r;r.name="growing";r.found=true;r.bytes=100;r.growthBytes=512ULL*1024*1024;r.location=std::filesystem::temp_directory_path()/"devclean-growing";const auto i=buildCacheInsights({r},{});assert(!i.recommendations.empty());assert(i.recommendations.front().priority==95);assert(i.recommendations.front().growthBytes==r.growthBytes);return 0;}
