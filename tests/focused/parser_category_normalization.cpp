#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--category",a3[]="Build";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.category=="build");return 0;}
