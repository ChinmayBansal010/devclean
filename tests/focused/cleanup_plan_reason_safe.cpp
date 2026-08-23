#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult r;r.name="safe";r.found=true;r.bytes=10;const auto p=buildCleanupPlan({r},0,false);assert(p.items.front().reason.find("inactive")!=std::string::npos);return 0;}
