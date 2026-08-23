#include "core/CacheInsights.hpp"
#include <cassert>
int main(){const auto i=buildCacheInsights({},{});assert(i.foundCount==0);assert(i.totalBytes==0);assert(i.health.score==100);assert(i.health.label=="Excellent");return 0;}
