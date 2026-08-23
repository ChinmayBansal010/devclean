#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="growing";a.found=true;a.bytes=50;a.growthBytes=1;ScanResult b;b.name="large";b.found=true;b.bytes=2ULL*1024*1024*1024;const auto p=buildCleanupPlan({a,b},0,false);assert(p.items.front().priority>=p.items.back().priority);return 0;}
