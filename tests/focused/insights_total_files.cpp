#include "core/CacheInsights.hpp"
#include <cassert>
int main(){ScanResult r;r.name="files";r.found=true;r.bytes=10;r.files=12;r.directories=3;const auto i=buildCacheInsights({r},{});assert(i.totalFiles==12);assert(i.totalDirectories==3);return 0;}
