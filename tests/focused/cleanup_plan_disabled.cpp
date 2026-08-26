#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="cache";r.bytes=100;r.found=true;r.enabled=false;r.location="/tmp/cache";const auto p=buildCleanupPlan({r},100,false);assert(p.items.empty());return 0;}
