#include "scanner/ScanResult.hpp"
#include <cassert>
int main(){ScanResult r;r.warnings.push_back("warning");assert(r.hasWarnings());return 0;}
