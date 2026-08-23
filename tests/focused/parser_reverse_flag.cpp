#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--reverse";char* v[]={a0,a1,a2};const auto a=ArgumentParser::parse(3,v);assert(a.reverse);return 0;}
