#ifndef HENSON_PYTHON_PUPPET_HPP
#define HENSON_PYTHON_PUPPET_HPP

#include <pybind11/embed.h>
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

            py::object scope = py::module::import("__main__").attr("__dict__");
            scope["henson_yield"]           = py::module::import("pyhenson").attr("yield_");
            scope["henson_get_array"]       = py::module::import("pyhenson").attr("get_array");
            scope["henson_get"]             = py::module::import("pyhenson").attr("get");
            scope["henson_add"]             = py::module::import("pyhenson").attr("add");
            scope["henson_create_queue"]    = py::module::import("pyhenson").attr("create_queue");
            scope["henson_queue_empty"]     = py::module::import("pyhenson").attr("queue_empty");
            scope["henson_exists"]          = py::module::import("pyhenson").attr("exists");
            scope["henson_clear"]           = py::module::import("pyhenson").attr("clear");
            scope["henson_stop"]            = py::module::import("pyhenson").attr("stop");

            // we can have only one PythonPuppet (otherwise scoped_interpreter
            // will complain), so there is only one pyhenson module, with only
            // one self to modify
            py::module::import("pyhenson").attr("self") = reinterpret_cast<intptr_t>(self_);

            self->running_ = true;
            self->start_time_ = get_time();
            try
            {
                py::eval_file(self->filename_, scope);
            } catch (const std::exception& e)
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

struct PyArray: public Array
{
    PyArray(Array a, std::string f):
        Array(a), format(f)             {}
    std::string format;
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

    // TODO: this is duplicating code with Python bindings; find a way to avoid code duplication
    py::class_<PyArray>(m,  "Array", py::buffer_protocol())
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

    m.def("get_array",  [nm](std::string name, std::string format) { return PyArray(nm()->get(name).a, format); });
    m.def("get",        [nm](std::string name, std::string format) -> py::object
                        {
                            if (format == "f")
                                return py::float_(nm()->get(name).f);
                            else if (format == "d")
                                return py::float_(nm()->get(name).d);
                            else if (format == "i")
                                return py::int_(nm()->get(name).i);
                            else if (format == "Q")
                                return py::int_(nm()->get(name).s);
                            else
                                throw py::cast_error("Unkown format: " + format);
                        });
    m.def("add",        [nm](std::string name, int    x)    { Value v; v.tag = Value::_int;    v.i = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, size_t x)    { Value v; v.tag = Value::_size_t; v.s = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, float  x)    { Value v; v.tag = Value::_float;  v.f = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, double x)    { Value v; v.tag = Value::_double; v.d = x; nm()->add(name, v); });

    m.def("create_queue",   [nm](std::string name)          { nm()->create_queue(name); });
    m.def("queue_empty",    [nm](std::string name)          { return nm()->queue_empty(name); });

    m.def("exists",     [nm](std::string name)      { return nm()->exists(name); });
    m.def("clear",      [nm]()                      { nm()->clear(); });
}

#endif
