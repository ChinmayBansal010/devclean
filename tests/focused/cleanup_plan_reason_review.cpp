#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="review";r.found=true;r.bytes=10;r.active=true;const auto p=buildCleanupPlan({r},0,false);assert(p.items.front().reason=="requires explicit review");return 0;}
