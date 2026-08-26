#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="cache";r.bytes=100;r.found=true;r.enabled=true;r.location="/tmp/cache";r.active=true;const auto p=buildCleanupPlan({r},100,true);assert(!p.targetReached);assert(p.items.empty());return 0;}
