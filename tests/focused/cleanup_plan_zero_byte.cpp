#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="empty";r.found=true;r.bytes=0;const auto p=buildCleanupPlan({r},0,false);assert(p.items.empty());return 0;}
