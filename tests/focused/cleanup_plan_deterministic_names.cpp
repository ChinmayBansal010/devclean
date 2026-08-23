#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult z;z.name="zeta";z.found=true;z.bytes=100;ScanResult a;a.name="alpha";a.found=true;a.bytes=100;const auto p=buildCleanupPlan({z,a},0,false);assert(p.items.front().name=="alpha");return 0;}
