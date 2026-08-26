#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="cache";r.bytes=100;r.found=true;r.enabled=true;r.location="/tmp/cache";const auto p=buildCleanupPlan({r},100,false);assert(p.targetReached);assert(p.plannedBytes==100);return 0;}
