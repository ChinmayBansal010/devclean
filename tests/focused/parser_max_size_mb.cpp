#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--max-size",a3[]="3MB";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.maxSizeBytes==3ULL*1024*1024);return 0;}
