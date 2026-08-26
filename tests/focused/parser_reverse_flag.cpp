#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean";char a1[]="scan";char a2[]="--reverse";char* argv[]={a0,a1,a2};const auto args=ArgumentParser::parse(3,argv);assert(args.reverse);return 0;}
