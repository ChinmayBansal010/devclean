#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="warning";r.found=true;r.bytes=200;r.warnings={"in use"};const auto p=buildCleanupPlan({r},0,true);assert(p.items.empty());return 0;}
