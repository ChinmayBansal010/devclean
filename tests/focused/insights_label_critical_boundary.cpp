#include "core/CacheInsights.hpp"
#include <cassert>
int main(){assert(cacheHealthLabel(40)=="At risk");assert(cacheHealthLabel(39)=="Critical");return 0;}
