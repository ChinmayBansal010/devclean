#include "core/CacheInsights.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=10;ScanResult b;b.name="b";b.found=false;b.bytes=20;const auto i=buildCacheInsights({a,b},{});assert(i.foundCount==1);assert(i.totalBytes==10);return 0;}
