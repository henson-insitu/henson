#include <vector>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/eval.h>
namespace py = pybind11;

#include <henson/puppet.hpp>
#include <henson/procs.hpp>
#include <henson/data.hpp>

#include <mpi.h>

#if defined(HENSON_MPI4PY)
#include "mpi-comm.h"
#endif

#include "mpi-capsule.h"

#include <henson/scheduler.hpp>

namespace henson {

void save_py_object(MemoryBuffer& bb, const py::object& o)
{
    // TODO: find a way to avoid this overhead on every call
    py::module pickle = py::module::import("pickle");
    py::object dumps  = pickle.attr("dumps");

    py::bytes data = dumps(o);
    auto data_str = std::string(data);

    save(bb, data_str);
}

void load_py_object(MemoryBuffer& bb, py::object& o)
{
    // TODO: find a way to avoid this overhead on every call
    py::module pickle = py::module::import("pickle");
    py::object loads  = pickle.attr("loads");

    std::string data_str;
    load(bb, data_str);

    py::bytes data_bytes = data_str;
    o = loads(data_bytes);
}


class PyScheduler : public Scheduler {
public:
    PyScheduler(henson::ProcMap* proc_map, int controller_ranks = 1):
        Scheduler(proc_map, controller_ranks) {}


    void _schedule(std::string name, std::string function, const py::object& arg, std::map<std::string, int> groups, int size)
    {
        ProcMap::Vector groups_vector;
        // divide unused procs between groups of size <= 0
        std::vector<std::string> unspecified;
        int specified = 0;
        for (auto& x : groups)
        {
            int sz = x.second;
            if (sz <= 0)
                unspecified.push_back(x.first);
            else
                specified += sz;
        }

        int leftover = size - specified;
        int leftover_group_size = size / unspecified.size();
        for (auto& x : groups)
        {
            int sz = x.second;
            if (sz > 0)
                groups_vector.emplace_back(x.first, sz);
            else if (x.first == unspecified.back())
                groups_vector.emplace_back(x.first, leftover - leftover_group_size * (unspecified.size() - 1));     // in case leftover doesn't divide evenly
            else
                groups_vector.emplace_back(x.first, leftover_group_size);
        }

        MemoryBuffer mb_arg;
        save_py_object(mb_arg, arg);
        schedule(name, function, mb_arg, groups_vector, size);
    }

    void _listen()
    {
        std::function<MemoryBuffer(Job&)> runner = [this](Job& job)
        {
            py::object scope = py::module::import("__main__").attr("__dict__");
            py::object func = py::module::import("__main__").attr(job.function.c_str());
            py::object res;

            job.arg.reset();
            py::object py_arg;
            load_py_object(job.arg, py_arg);
            res = func(py_arg);

            MemoryBuffer result;

            if (!res.is_none()) {
                save_py_object(result, res);
            }

            return result;
        };

       listen(runner);
    }

    py::object pop_python()
    {
        MemoryBuffer mb = pop();
        mb.reset();
        py::object result;
        load_py_object(mb, result);
        return result;
    }
};
} // namespace henson

PYBIND11_MODULE(pyhenson, m)
{
    m.doc() = "Henson bindings";


#if defined(HENSON_MPI4PY)
    // import the mpi4py API
    if (import_mpi4py() < 0)
        throw std::runtime_error("Could not load mpi4py API.");
#endif

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

                                    //fmt::print("{}\n", arguments.back());
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
        .def("__init__", [](ProcMap& pm)
                         {
                             MPI_Comm world;
                             MPI_Comm_dup(MPI_COMM_WORLD, &world);
                             int size;
                             MPI_Comm_size(world, &size);
                             std::cerr << "in ProcMap, size = " << size << std::endl;
                             ProcMap::Vector v = ProcMap::parse_procs({"world"}, size);
                             std::cerr << "in ProcMap, v.size = " << v.size() << std::endl;
                             new (&pm) ProcMap(world, v);
                         })
        .def("__init__", [](ProcMap& pm, ProcMap::Vector v)
                         {
                            MPI_Comm comm;
                            MPI_Comm_dup(MPI_COMM_WORLD, &comm);
                            new (&pm) ProcMap(comm, v);
                         })
        .def("__init__", [](ProcMap& pm, py::capsule comm, ProcMap::Vector v)
                         {
                            new (&pm) ProcMap(from_capsule<MPI_Comm>(comm), v);
                         })
#if defined(HENSON_MPI4PY)
        .def("__init__", [](ProcMap& pm, mpi4py_comm comm, ProcMap::Vector v)
                         {
                            new (&pm) ProcMap(comm, v);
                         })
#endif
        .def("local", [](const ProcMap& pm)
                      {
                          return to_capsule(pm.local());
                      })
        .def("local_rank", &ProcMap::local_rank)
        .def("world", [](const ProcMap& pm)
                      {
                          return to_capsule(pm.world());
                      })
        .def("group", &ProcMap::group)
        .def("intercomm", [](ProcMap& pm, const std::string& to, int tag = 0)
                          {
                            return to_capsule(pm.intercomm(to, tag));
                          })
       ;

//    m.def("get_procmap",

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
                                    py::object operator()(Array a) const    { return visit(extract_array { a.count, a.stride }, a.address); }
                                    py::object operator()(std::string x) const    { return py::str(x); }
                                };
                                return visit(extract{}, nm.get(name));
                            })
        .def("exists",      &NameMap::exists)
        .def("clear",       &NameMap::clear);

    m.def("clock_to_string",  &clock_to_string);
    m.def("world_size",       []() { int size; MPI_Comm_size(MPI_COMM_WORLD, &size); return size; });

     py::class_<Scheduler>(m, "BaseScheduler")
        .def("__init__",    [](Scheduler& ps, ProcMap& procmap, int controller_ranks)
         {
              new (&ps) Scheduler(&procmap, controller_ranks);
         })
        .def("size",                &Scheduler::size)
        .def("rank",                &Scheduler::rank)
        .def("workers",             &Scheduler::workers)
        .def("job_queue_empty",     &Scheduler::job_queue_empty)
        .def("is_controller",       &Scheduler::is_controller)
        .def("control",             &Scheduler::control)
        .def("results_empty",       &Scheduler::results_empty)
        .def("finish",              &Scheduler::finish)
        ;

    py::class_<PyScheduler, Scheduler>(m, "Scheduler")
        .def("__init__",    [](PyScheduler& ps, ProcMap& procmap, int controller_ranks)
         {
              new (&ps) PyScheduler(&procmap, controller_ranks);
         })
        .def("listen",              &PyScheduler::_listen)
        .def("schedule",            &PyScheduler::_schedule)
        .def("pop",                 &PyScheduler::pop_python)
        ;

//    PyScheduler(MPI_Comm world,  henson::ProcMap* proc_map, int controller_ranks = 1):

#if defined(HENSON_MPI4PY)
    m.def("to_mpi4py",      [](py::capsule c)    -> mpi4py_comm  { return from_capsule<MPI_Comm>(c); });
    m.def("from_mpi4py",    [](mpi4py_comm comm) -> py::capsule  { return to_capsule(static_cast<MPI_Comm>(comm)); });
#endif
}
