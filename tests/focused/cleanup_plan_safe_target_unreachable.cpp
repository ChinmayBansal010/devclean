#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="safe";a.found=true;a.bytes=80;ScanResult b;b.name="active";b.found=true;b.bytes=100;b.active=true;const auto p=buildCleanupPlan({a,b},150,true);assert(p.plannedBytes==80);assert(!p.targetReached);return 0;}
