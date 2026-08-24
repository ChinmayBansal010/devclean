#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="growth-large";r.location="/tmp/devclean-test";r.found=true;r.bytes=2ULL*1024*1024*1024;r.growthBytes=1;const auto p=buildCleanupPlan({r},0,true);assert(p.items.front().priority==130);return 0;}
