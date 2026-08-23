#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="clean",a2[]="--force";char* v[]={a0,a1,a2};const auto a=ArgumentParser::parse(3,v);assert(a.force);return 0;}
