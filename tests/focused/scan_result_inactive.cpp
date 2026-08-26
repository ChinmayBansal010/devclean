#include "scanner/ScanResult.hpp"
#include <cassert>
int main(){ScanResult r;r.active=false;assert(r.isInactive());return 0;}
