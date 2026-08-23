#include "core/CacheInsights.hpp"
#include <cassert>
#include <filesystem>
int main(){ScanResult r;r.name="large-safe";r.found=true;r.bytes=1024ULL*1024*1024;r.location=std::filesystem::temp_directory_path()/"devclean-large-safe";const auto i=buildCacheInsights({r},{});assert(!i.recommendations.empty());assert(i.recommendations.front().safe);assert(i.recommendations.front().priority==100);return 0;}
