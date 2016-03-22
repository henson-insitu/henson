#include <string>

#include <mpi.h>

#include <henson/data.h>
#include <henson/data.hpp>
#include <henson/context.h>
namespace h = henson;

#include <henson/time.hpp>

#include <format.h>
#include <opts/opts.h>

std::string     timer(std::string name)                     { return "timer_" + name; }
std::string     timer_start(std::string name)               { return "timer_start_" + name; }

void            save_time(std::string name, h::time_type t) { h::save(name, new h::Value<h::time_type>(t)); }
h::time_type    load_time(std::string name)                 { return h::load< h::Value<h::time_type> >(name)->value; }

h::time_type    get_henson_timer(std::string name)
{
    if (!henson::exists(timer(name))) return 0;
    return load_time(timer(name));
}

int main(int argc, char** argv)
{
    if (!henson_active())
    {
        fmt::print("{} should run under henson\n", argv[0]);
        return 1;
    }
    MPI_Comm world = MPI_COMM_WORLD;
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


        save_time(timer_start(name), henson::get_time());
    } else if (cmd == "stop")
    {
        std::string name;
        ops >> PosOption(name);

        h::time_type start = load_time(timer_start(name));
        h::time_type elapsed = henson::get_time() - start;

        if (!increment)
            save_time(timer(name), elapsed);
        else
        {
            h::time_type cur = get_henson_timer(name);
            save_time(timer(name), cur + elapsed);
        }
    } else if (cmd == "reset")
    {
        std::string name;
        ops >> PosOption(name);
        save_time(timer(name), 0);
    } else if (cmd == "<" || cmd == "<=" || cmd == ">" || cmd == ">=")
    {
        std::string name1, name2;
        ops >> PosOption(name1) >> PosOption(name2);

        h::time_type t1 = get_henson_timer(name1),
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
    } else if (cmd == "report")
    {
        std::string name;
        ops >> PosOption(name);

        h::time_type start = load_time(timer_start(name));
        h::time_type elapsed = henson::get_time() - start;

        fmt::print("Elapsed: {}\n", h::clock_to_string(elapsed));
    }
}
