#include "core/CacheInsights.hpp"
#include <cassert>
#include <chrono>
int main(){std::vector<ScanSnapshot> h;for(int i=0;i<8;++i){ScanSnapshot s;s.timestamp=std::chrono::system_clock::now()-std::chrono::hours(24*i);s.totalBytes=static_cast<uint64_t>(i+1)*100;h.push_back(s);}const auto i=buildCacheInsights({},h);assert(i.history.size()==6);return 0;}
