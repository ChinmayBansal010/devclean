#include "core/CacheInsights.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=10;a.warnings={"one","two"};const auto i=buildCacheInsights({a},{});assert(i.warningCount==2);return 0;}
