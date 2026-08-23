#include "core/CacheInsights.hpp"
#include <cassert>
int main(){assert(cacheHealthLabel(60)=="Watch");assert(cacheHealthLabel(59)=="At risk");return 0;}
