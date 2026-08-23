#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="busy";r.found=true;r.bytes=1;r.active=true;r.warnings={"busy"};const auto p=buildCleanupPlan({r},0,false);assert(p.items.front().priority==0);return 0;}
