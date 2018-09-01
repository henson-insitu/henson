#ifndef HENSON_PYTHON_PUPPET_HPP
#define HENSON_PYTHON_PUPPET_HPP

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include "coroutine.hpp"

namespace henson
{
struct PythonPuppet: public Coroutine<PythonPuppet>
{
    using Parent = Coroutine<PythonPuppet>;

                        PythonPuppet(const std::string& filename, ProcMap* procmap, NameMap* namemap):
                            Parent(filename),
                            filename_(filename), procmap_(procmap), namemap_(namemap)
                        {}

                        ~PythonPuppet()
                        {
                            if(running_)
                            {
                                signal_stop();
                                proceed();
                            }
                        }

    static void         exec(void* self_)
    {
        PythonPuppet* self = (PythonPuppet*) self_;

        while(true)
        {
            py::scoped_interpreter guard;

            // we can have only one PythonPuppet (otherwise scoped_interpreter
            // will complain), so there is only one pyhenson module, with only
            // one self to modify
            py::module::import("pyhenson").attr("self") = reinterpret_cast<intptr_t>(self_);

            self->running_ = true;
            self->start_time_ = get_time();
            try
            {
                py::eval_file(self->filename_);
            } catch (const py::error_already_set& e)
            {
                self->log_->info("Caught error_already_set (maybe SystemExit; check debug channel for details)");
                self->log_->debug("  {}", e.what());
            }
            catch (const std::exception& e)
            {
                self->log_->warn("Got exception from Python: {}", e.what());
            }
            self->running_ = false;
            self->yield();      // the time for the final portion will get recorded thanks to this call
        }
    }

    std::string         filename_;
    ProcMap*            procmap_;
    NameMap*            namemap_;
};

}

PYBIND11_EMBEDDED_MODULE(pyhenson, m)
{
    using namespace henson;

    auto self = []()
    {
        auto self__ = py::module::import("pyhenson").attr("self");
        void* self_ = reinterpret_cast<void*>(self__.cast<intptr_t>());
        PythonPuppet* self = (PythonPuppet*) self_;
        return self;
    };

    auto nm = [self]()
    {
        NameMap*      nm   = self()->namemap_;
        return nm;
    };

    m.def("yield_",     [self]() { self()->yield(); });
    m.def("stop",       [self]() { return self()->stop_ != 0; });

    m.def("get",        [nm](std::string name) -> py::object
                        {
                            struct extract
                            {
                                py::object operator()(int x) const      { return py::int_(x); }
                                py::object operator()(size_t x) const   { return py::int_(x); }
                                py::object operator()(float x) const    { return py::float_(x); }
                                py::object operator()(double x) const   { return py::float_(x); }
                                py::object operator()(void* x) const    { throw  py::cast_error("Cannot return void* to Python"); }
                                py::object operator()(Array a) const
                                {
                                    if (a.type == sizeof(float))
                                        return py::array_t<float>({ a.count }, { a.stride }, static_cast<float*>(a.address));
                                    else if (a.type == sizeof(double))
                                        return py::array_t<double>({ a.count }, { a.stride }, static_cast<double*>(a.address));
                                    else
                                        throw py::cast_error("Unknown type: " + std::to_string(a.type));
                                }
                            };
                            return mpark::visit(extract{}, nm()->get(name));
                        });
    m.def("add",        [nm](std::string name, int    x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, size_t x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, float  x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, double x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, const py::array& x)
    {
        size_t type;
        if (x.dtype().is(py::dtype::of<float>()))
            type = sizeof(float);
        else if (x.dtype().is(py::dtype::of<double>()))
            type = sizeof(double);
        else
            throw std::runtime_error("Unknown array dtype");

        Value v = henson::Array(const_cast<void*>(x.data()), type, x.size(), type);
        nm()->add(name, v);
    });

    m.def("create_queue",   [nm](std::string name)          { nm()->create_queue(name); });
    m.def("queue_empty",    [nm](std::string name)          { return nm()->queue_empty(name); });

    m.def("exists",     [nm](std::string name)      { return nm()->exists(name); });
    m.def("clear",      [nm]()                      { nm()->clear(); });
}

#endif
