#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="review";r.found=true;r.bytes=200;r.active=true;const auto p=buildCleanupPlan({r},0,false);assert(p.items.size()==1);assert(!p.items.front().safe);assert(p.items.front().reason=="requires explicit review");return 0;}
