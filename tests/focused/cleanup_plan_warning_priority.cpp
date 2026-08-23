#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="warning";a.found=true;a.bytes=100;a.warnings={"busy"};ScanResult b;b.name="plain";b.found=true;b.bytes=100;const auto p=buildCleanupPlan({a,b},0,false);assert(p.items.back().name=="warning");return 0;}
