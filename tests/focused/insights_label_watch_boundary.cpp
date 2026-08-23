#include "core/CacheInsights.hpp"
#include <cassert>
int main(){assert(cacheHealthLabel(75)=="Healthy");assert(cacheHealthLabel(74)=="Watch");return 0;}
