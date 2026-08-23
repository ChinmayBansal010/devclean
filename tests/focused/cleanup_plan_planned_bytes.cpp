#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=30;ScanResult b;b.name="b";b.found=true;b.bytes=70;const auto p=buildCleanupPlan({a,b},0,false);assert(p.plannedBytes==100);return 0;}
