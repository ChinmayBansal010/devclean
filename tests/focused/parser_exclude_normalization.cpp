#include "core/ArgumentParser.hpp"
#include <cassert>
int main(){char a0[]="devclean",a1[]="scan",a2[]="--exclude",a3[]="Bazel";char* v[]={a0,a1,a2,a3};const auto a=ArgumentParser::parse(4,v);assert(a.excludes.size()==1);assert(a.excludes.front()=="bazel");return 0;}
