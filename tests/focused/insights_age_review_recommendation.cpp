#include "core/CacheInsights.hpp"
#include <cassert>
#include <chrono>
#include <filesystem>
int main(){ScanResult r;r.name="old";r.found=true;r.bytes=100ULL*1024*1024;r.age=std::chrono::hours(24*31);r.location=std::filesystem::temp_directory_path()/"devclean-old";const auto i=buildCacheInsights({r},{});assert(!i.recommendations.empty());assert(i.recommendations.front().priority==50);return 0;}
