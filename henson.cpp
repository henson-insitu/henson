#include <mpi.h>

#include <opts/opts.h>
#include <format.h>

#include <henson/puppet.hpp>
#include <henson/data.hpp>


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    MPI_Comm world = MPI_COMM_WORLD;

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    fmt::print("[{}]: henson started; total processes = {}\n", rank, size);

    using namespace opts;
    Options ops(argc, argv);

    // TODO: parse a rudimentary script describing the puppets

    std::string simulation_fn, analysis_fn;
    if (  ops >> Present('h', "help", "show help") ||
        !(ops >> PosOption(simulation_fn) >> PosOption(analysis_fn)))
    {
        fmt::print("Usage: {} SIMULATION ANALYSIS\n{}", argv[0], ops);
        return 1;
    }

    henson::NameMap     namemap;

    henson::Puppet  simulation(simulation_fn, 0, 0, world, &namemap);
    henson::Puppet  analysis  (analysis_fn,   0, 0, world, &namemap);

    do
    {
        simulation.proceed();
        if (!simulation.running())
            break;

        analysis.proceed();
    } while (true);

    fmt::print("[{}]: henson done\n", rank);

    MPI_Finalize();
}
