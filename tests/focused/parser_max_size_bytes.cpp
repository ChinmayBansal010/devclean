#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--max-size",a3[]="4096";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.maxSizeBytes==4096);return 0;}
