#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean";char a1[]="clean";char a2[]="--safe";char a3[]="--dry-run";char* argv[]={a0,a1,a2,a3};const auto args=ArgumentParser::parse(4,argv);assert(args.safe);assert(args.dryRun);return 0;}
