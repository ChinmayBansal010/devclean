#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="missing";r.found=false;r.bytes=100;const auto p=buildCleanupPlan({r},0,false);assert(p.items.empty());return 0;}
