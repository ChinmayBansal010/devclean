#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="large";a.found=true;a.bytes=2ULL*1024*1024*1024;ScanResult b;b.name="small";b.found=true;b.bytes=100;const auto p=buildCleanupPlan({a,b},0,false);assert(p.items.front().name=="large");assert(p.items.front().priority>p.items.back().priority);return 0;}
