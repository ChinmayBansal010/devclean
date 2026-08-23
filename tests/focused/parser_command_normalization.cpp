#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="SCAN";char* v[]={a0,a1};const auto a=ArgumentParser::parse(2,v);assert(a.command=="scan");return 0;}
