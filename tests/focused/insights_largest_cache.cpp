#include "core/CacheInsights.hpp"
#include <cassert>
int main(){ScanResult a;a.name="small";a.found=true;a.bytes=10;ScanResult b;b.name="large";b.found=true;b.bytes=100;const auto i=buildCacheInsights({a,b},{});assert(i.largestCaches.front().first=="large");assert(i.largestCaches.front().second==100);return 0;}
