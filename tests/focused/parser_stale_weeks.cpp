#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="clean",a2[]="--stale",a3[]="1w";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.staleSeconds==7ULL*24*60*60);return 0;}
