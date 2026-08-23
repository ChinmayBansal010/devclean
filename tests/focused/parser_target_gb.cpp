#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="clean",a2[]="--target",a3[]="4GB";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.targetSizeBytes==4ULL*1024*1024*1024);return 0;}
