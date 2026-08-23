#include "core/CacheInsights.hpp"
#include <cassert>
#include <filesystem>
int main(){ScanResult a;a.name="review";a.found=true;a.bytes=600ULL*1024*1024;a.location=std::filesystem::temp_directory_path()/"devclean-review";ScanResult b;b.name="safe";b.found=true;b.bytes=1ULL*1024*1024*1024;b.location=std::filesystem::temp_directory_path()/"devclean-safe";const auto i=buildCacheInsights({a,b},{});assert(i.recommendations.size()>=2);assert(i.recommendations.front().priority>=i.recommendations.back().priority);return 0;}
