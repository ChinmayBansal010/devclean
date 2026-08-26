#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="cache";r.bytes=100;r.found=true;r.enabled=true;r.location="/tmp/cache";const auto p=buildCleanupPlan({r},100,true);assert(p.targetReached);assert(p.items.size()==1);assert(p.items[0].safe);return 0;}
