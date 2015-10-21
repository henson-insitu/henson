#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <format.h>

#include "mpi.hpp"

PYBIND11_PLUGIN(pympi)
{
    py::module m("pympi", "MPI bindings");

    // a hack for now, until we can interface with mpi4py properly
    py::class_<communicator>(m, "communicator")
        .def(py::init<>())
        .def(py::init<MPI_Comm>())
        .def("rank", &communicator::rank)
        .def("size", &communicator::size)
        .def("__repr__", [](const communicator& c) { return fmt::format("MPI Communicator: rank = {}; size = {}", c.rank(), c.size()); });

    py::class_<environment>(m, "environment")
        .def("__repr__", [](const environment&) { return "MPI Environment"; });

    m.attr("env") = py::cast(new environment);

    return m.ptr();
}
