#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="large-safe";r.found=true;r.bytes=2ULL*1024*1024*1024;const auto p=buildCleanupPlan({r},0,true);assert(p.items.front().priority==110);return 0;}
