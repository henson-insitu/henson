#include <vector>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <henson/puppet.hpp>
#include <henson/procs.hpp>
#include <henson/data.hpp>

#include <mpi.h>

struct PyArray: public henson::Array
{
    PyArray(henson::Array a, std::string f):
        henson::Array(a), format(f)             {}
    std::string format;
};

PYBIND11_PLUGIN(pyhenson)
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
        .def("__init__", [](ProcMap& pm, long comm_, ProcMap::Vector v)
                         {
                            MPI_Comm comm = *static_cast<MPI_Comm*>(reinterpret_cast<void*>(comm_));
                            new (&pm) ProcMap(comm, v);
                         });

    py::class_<PyArray>(m,   "Array")
        .def_buffer([](PyArray& a) -> py::buffer_info
                    {
                        return py::buffer_info
                               { a.address,
                                 a.type,
                                 a.format,
                                 1,
                                 { a.count },
                                 { a.stride }
                               };
                    });

    py::class_<NameMap>(m, "NameMap")
        .def(py::init<>())
        .def("get_array",   [](const NameMap& nm, std::string name, std::string format) { return PyArray(*nm.get<Array>(name), format); })
        .def("get",         [](const NameMap& nm, std::string name, std::string format) -> py::object
                            {
                                if (format == "f")
                                    return py::float_(nm.get<Value<float>>(name)->value);
                                else if (format == "d")
                                    return py::float_(nm.get<Value<double>>(name)->value);
                                else if (format == "i")
                                    return py::int_(nm.get<Value<int>>(name)->value);
                                else if (format == "I")
                                    return py::int_(nm.get<Value<uint32_t>>(name)->value);
                                else if (format == "q")
                                    return py::int_(nm.get<Value<int64_t>>(name)->value);
                                else if (format == "Q")
                                    return py::int_(nm.get<Value<uint64_t>>(name)->value);
                                else
                                    throw py::cast_error("Unkown format: " + format);
                            })
        .def("exists",      &NameMap::exists)
        .def("clear",       &NameMap::clear);

    m.def("clock_to_string",  &clock_to_string);

    return m.ptr();
}
