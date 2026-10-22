#include "test-xnu.h"
#include <cstdarg>
#include <vector>
#include <set>
#include <algorithm>
static std::atomic<uint64_t> fake_ms{1000};
static thread_local bool script_clock=false,signal_clock=false;
static thread_local unsigned script_pos;
static thread_local uint64_t script_times[3];
static std::atomic<bool> clock_entered{false};
static uint64_t timing_now(){
 if(script_clock){assert(script_pos<3);return script_times[script_pos++];}
 uint64_t value=fake_ms.load();if(signal_clock)clock_entered=true;return value;
}
static std::mutex log_lock;
static std::vector<std::string> captured;
static int timing_printf(const char*f,...){char line[2048];va_list a;va_start(a,f);int n=vsnprintf(line,sizeof line,f,a);va_end(a);assert(n>=0&&(size_t)n<sizeof line);
 if(!strncmp(line,"NORD2 WIFI IOS QUERY",20)){assert(!timing_locks_held);assert(n<=255);std::lock_guard<std::mutex>g(log_lock);captured.emplace_back(line);}
 return n;
}
#define NIWL_TIMING_TEST_NOW timing_now
#define printf timing_printf
