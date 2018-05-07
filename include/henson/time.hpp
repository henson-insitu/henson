#ifndef HENSON_TIME_HPP
#define HENSON_TIME_HPP

#include <string>

#include <sys/time.h>

#include <fmt/format.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

namespace henson
{

typedef             unsigned long long                  time_type;

inline
time_type
get_time()
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t ts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &ts);
    mach_port_deallocate(mach_task_self(), cclock);
#else
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
#endif
    //return ts.tv_sec*1000 + ts.tv_nsec/1000000;

    time_type result = ts.tv_sec;
    result *= 1000000000;
    result += ts.tv_nsec;
    return result;
}

inline
std::string
clock_to_string(time_type time)
{
    time /= 1000;       // microsecond
    return fmt::format("{:02}:{:02}:{:02}.{:06}",
                        time/1000000/60/60,
                        time/1000000/60 % 60,
                        time/1000000 % 60,
                        time % 1000000);
}

}

#endif
