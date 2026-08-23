#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="Bazel",a3[]="Pip";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.targets.size()==2);assert(a.targets[0]=="bazel");assert(a.targets[1]=="pip");return 0;}
