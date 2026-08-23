#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="active";r.found=true;r.bytes=200;r.active=true;const auto p=buildCleanupPlan({r},0,true);assert(p.items.empty());return 0;}
