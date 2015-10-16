#include <vector>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <henson/puppet.hpp>
#include <henson/procs.hpp>
#include <henson/data.hpp>

PYBIND_PLUGIN(pyhenson)
{
    py::module m("pyhenson", "Henson bindings");

    using namespace henson;

    py::class_<Puppet>(m, "Puppet")
        .def("__init__",    [](Puppet& p, std::string filename, std::vector<std::string> args, ProcMap& procmap, NameMap& namemap)
                            {
                                int argc = args.size();
                                std::vector<char*>  arguments;

                                arguments.emplace_back(new char[filename.size() + 1]);
                                std::copy(filename.begin(), filename.end(), arguments.back());
                                arguments.back()[filename.size()] = '\0';

                                for (auto arg : args)
                                {
                                    arguments.emplace_back(new char[arg.size() + 1]);
                                    std::copy(arg.begin(), arg.end(), arguments.back());
                                    arguments.back()[arg.size()] = '\0';

                                    fmt::print("{}\n", arguments.back());
                                }

                                new (&p) Puppet(filename, argc + 1, &arguments[0], &procmap, &namemap);

                                for (char* a : arguments)
                                    delete[] a;
                            })
        .def("proceed",     &Puppet::proceed,           "continue execution of the puppet")
        //.def("yield",       &Puppet::yield)
        .def("signal_stop", &Puppet::signal_stop,       "signal to the puppet that execution will stop after this round")
        .def("running",     &Puppet::running,           "check if the puppet is running")
        .def("result",      &Puppet::result,            "check the last return value of the puppet")
        .def("total_time",  &Puppet::total_time,        "total time the puppet has been running")
        .def("filename",    [](const Puppet& p) -> std::string
                            { return p.filename_; },    "executable filename ")
        .def("__str__",     [](const Puppet& p) -> std::string
                            { return "Puppet: " + p.filename_; });

    py::class_<ProcMap>(m, "ProcMap")
        .def(py::init<MPI_Comm, ProcMap::Vector>());

    py::class_<NameMap>(m, "NameMap")
        .def(py::init<>());

    m.def("clock_to_string",  &clock_to_string);

    // a hack for now, until we can interface with mpi4py properly
    m.def("MPI_COMM_WORLD", []() -> MPI_Comm    { return MPI_COMM_WORLD; });
    m.def("MPI_Init",   []()                    { MPI_Init(0,0); });
    m.def("MPI_Finalize", []()                  { MPI_Finalize(); });

    return m.ptr();
}
