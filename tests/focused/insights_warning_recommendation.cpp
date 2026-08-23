#include "core/CacheInsights.hpp"
#include <cassert>
#include <filesystem>
int main(){ScanResult r;r.name="warning";r.found=true;r.bytes=100;r.warnings={"busy"};r.location=std::filesystem::temp_directory_path()/"devclean-warning";const auto i=buildCacheInsights({r},{});assert(!i.recommendations.empty());assert(!i.recommendations.front().safe);assert(i.recommendations.front().priority==70);return 0;}
