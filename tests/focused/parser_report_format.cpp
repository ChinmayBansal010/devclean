#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean";char a1[]="analyze";char a2[]="--report=json";char* argv[]={a0,a1,a2};const auto args=ArgumentParser::parse(3,argv);assert(args.reportFormat=="json");return 0;}
