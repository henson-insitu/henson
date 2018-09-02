#include <vector>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <henson/puppet.hpp>
#include <henson/procs.hpp>
#include <henson/data.hpp>

#include <mpi.h>

PYBIND11_MODULE(pyhenson, m)
{
    m.doc() = "Henson bindings";

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
        .def("__init__", [](ProcMap& pm, long comm_, ProcMap::Vector v)
                         {
                            MPI_Comm comm = *static_cast<MPI_Comm*>(reinterpret_cast<void*>(comm_));
                            new (&pm) ProcMap(comm, v);
                         });

    py::class_<NameMap>(m, "NameMap")
        .def(py::init<>())
        .def("get",         [](NameMap& nm, std::string name) -> py::object
                            {
                                struct extract_array
                                {
                                    py::object   operator()(float* a) const     { return py::array_t<float>({ count }, { stride }, a); }
                                    py::object   operator()(double* a) const    { return py::array_t<double>({ count }, { stride }, a); }
                                    py::object   operator()(int* a) const       { return py::array_t<int>({ count }, { stride }, a); }
                                    py::object   operator()(long* a) const      { return py::array_t<long>({ count }, { stride }, a); }
                                    py::object   operator()(void*) const        { throw py::cast_error("Cannot convert void* array to NumPy"); }

                                    size_t count;
                                    size_t stride;
                                };

                                struct extract
                                {
                                    py::object operator()(int x) const      { return py::int_(x); }
                                    py::object operator()(size_t x) const   { return py::int_(x); }
                                    py::object operator()(float x) const    { return py::float_(x); }
                                    py::object operator()(double x) const   { return py::float_(x); }
                                    py::object operator()(void* x) const    { throw  py::cast_error("Cannot return void* to Python"); }
                                    py::object operator()(Array a) const    { return mpark::visit(extract_array { a.count, a.stride }, a.address); }
                                };
                                return mpark::visit(extract{}, nm.get(name));
                            })
        .def("exists",      &NameMap::exists)
        .def("clear",       &NameMap::clear);

    m.def("clock_to_string",  &clock_to_string);
}
