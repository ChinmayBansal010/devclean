#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="safe";r.found=true;r.bytes=200;r.active=false;const auto p=buildCleanupPlan({r},0,true);assert(p.items.size()==1);assert(p.items.front().safe);assert(p.items.front().reason=="inactive and unprotected");return 0;}
