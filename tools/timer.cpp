#include <string>

#include <mpi.h>

#include <henson/data.h>
#include <henson/data.hpp>
#include <henson/context.h>

#include <henson/time.hpp>

#include <format.h>
#include <opts/opts.h>

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
    if (!henson_active())
    {
        fmt::print("{} should run under henson\n", argv[0]);
        return 1;
    }
    MPI_Comm world = henson_get_world();
    int rank;
    MPI_Comm_rank(world, &rank);

    using namespace opts;
    Options ops(argc, argv);

    bool increment = ops >> Present('i', "increment", "increment the timer value");
    bool root      = ops >> Present('r', "root",      "perform comparison on the root node only (and broadcast decision to the rest)");

    std::string cmd;
    ops >> PosOption(cmd);
    if (cmd == "start")
    {
        std::string name;
        ops >> PosOption(name);

        henson_save_size_t(timer_start(name).c_str(), henson::get_time());
    } else if (cmd == "stop")
    {
        std::string name;
        ops >> PosOption(name);

        size_t start;
        henson_load_size_t(timer_start(name).c_str(), &start);
        size_t elapsed = henson::get_time() - start;

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

        int res;
        if (!root || rank == 0)
        {
            if (cmd == "<" && t1 < t2)
                res = 0;
            else if (cmd == "<=" && t1 <= t2)
                res = 0;
            else if (cmd == ">"  && t1 >  t2)
                res = 0;
            else if (cmd == ">=" && t1 >= t2)
                res = 0;
            else
                res = 1;

            if (root)
                MPI_Bcast(&res, 1, MPI_INT, 0, world);
        } else
            MPI_Bcast(&res, 1, MPI_INT, 0, world);

        return res;
    }
}
