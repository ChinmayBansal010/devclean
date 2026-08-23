#include "core/CacheInsights.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=10;a.active=true;ScanResult b;b.name="b";b.found=true;b.bytes=20;const auto i=buildCacheInsights({a,b},{});assert(i.activeCount==1);return 0;}
