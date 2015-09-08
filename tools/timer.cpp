#include <string>

#include <sys/time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <mpi.h>

#include <henson/data.h>
#include <henson/data.hpp>
#include <henson/context.h>

#include <format.h>
#include <opts/opts.h>

typedef             unsigned long                       time_type;

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
    return ts.tv_sec*1000 + ts.tv_nsec/1000000;
}

std::string timer(std::string name)                 { return "timer_" + name; }
std::string timer_start(std::string name)           { return "timer_start_" + name; }

size_t      get_henson_timer(std::string name)
{
    if (!henson::exists(timer(name))) return 0;

    size_t t;
    henson_load_size_t(timer(name).c_str(), &t);

    return t;
}

int main(int argc, char** argv)
{
    using namespace opts;
    Options ops(argc, argv);

    bool increment = ops >> Present('i', "increment", "increment the timer value");

    std::string cmd;
    ops >> PosOption(cmd);
    if (cmd == "start")
    {
        std::string name;
        ops >> PosOption(name);

        henson_save_size_t(timer_start(name).c_str(), get_time());
    } else if (cmd == "stop")
    {
        std::string name;
        ops >> PosOption(name);

        size_t start;
        henson_load_size_t(timer_start(name).c_str(), &start);
        size_t elapsed = get_time() - start;

        if (!increment)
            henson_save_size_t(timer(name).c_str(), elapsed);
        else
        {
            size_t cur = get_henson_timer(name);
            henson_save_size_t(timer(name).c_str(), cur + elapsed);
        }
    } else if (cmd == "reset")
    {
        std::string name;
        ops >> PosOption(name);
        henson_save_size_t(timer(name).c_str(), 0);
    } else if (cmd == "<" || cmd == "<=" || cmd == ">" || cmd == ">=")
    {
        std::string name1, name2;
        ops >> PosOption(name1) >> PosOption(name2);

        size_t t1 = get_henson_timer(name1),
               t2 = get_henson_timer(name2);

        if (cmd == "<" && t1 < t2)
            return 0;
        else if (cmd == "<=" && t1 <= t2)
            return 0;
        else if (cmd == ">"  && t1 >  t2)
            return 0;
        else if (cmd == ">=" && t1 >= t2)
            return 0;
        else
            return 1;
    }
}
