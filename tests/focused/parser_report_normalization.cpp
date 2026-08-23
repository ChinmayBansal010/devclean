#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--report",a3[]="HTML";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.reportFormat=="html");return 0;}
