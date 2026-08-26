#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,600,125);assert(b.reclaimableBytes==125);assert(b.projectedBytes==475);return 0;}
