#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="active";a.found=true;a.bytes=100;a.active=true;ScanResult b;b.name="safe";b.found=true;b.bytes=100;const auto p=buildCleanupPlan({a,b},0,false);assert(p.items.back().name=="active");return 0;}
