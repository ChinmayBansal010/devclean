#include "core/CacheInsights.hpp"
#include <cassert>
#include <filesystem>
int main(){ScanResult r;r.name="large";r.found=true;r.bytes=600ULL*1024*1024;r.location=std::filesystem::temp_directory_path()/"devclean-large";const auto i=buildCacheInsights({r},{});assert(!i.recommendations.empty());assert(i.recommendations.front().priority==55);assert(i.recommendations.front().safe);return 0;}
