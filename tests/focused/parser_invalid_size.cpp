#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--min-size",a3[]="bad";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.minSizeBytes==0);return 0;}
