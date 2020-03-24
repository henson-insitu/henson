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
                        {
                            static std::unique_ptr<py::scoped_interpreter> guard;
                            if (!guard)
                                guard = decltype(guard)(new py::scoped_interpreter);
                        }

                        PythonPuppet(const std::string& filename, std::vector<std::string> arguments, ProcMap* procmap, NameMap* namemap):
                            PythonPuppet(filename, procmap, namemap)
                        {
                            arguments_ = std::move(arguments);
                        }

                        PythonPuppet(const std::string& filename, std::vector<char*> arguments, ProcMap* procmap, NameMap* namemap):
                            PythonPuppet(filename, procmap, namemap)
                        {
                            for (auto& arg : arguments)
                                arguments_.emplace_back(arg);
                        }

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
            {
                // we can have only one PythonPuppet (otherwise scoped_interpreter
                // will complain), so there is only one pyhenson module, with only
                // one self to modify
                py::module::import("pyhenson").attr("self") = reinterpret_cast<intptr_t>(self_);

                self->running_ = true;
                self->start_time_ = get_time();
                try
                {
                    auto argv = py::module::import("sys").attr("argv");
                    argv.attr("clear")();
                    for(size_t i = 0; i < self->arguments_.size(); ++i)
                    {
                        auto& arg = self->arguments_[i];
                        argv.attr("append")(arg);
                    }

                    py::dict locals;
                    py::eval_file(self->filename_, py::globals(), locals);
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
            }

            self->yield();      // the time for the final portion will get recorded thanks to this call
        }
    }

    std::string                 filename_;
    ProcMap*                    procmap_;
    NameMap*                    namemap_;
    std::vector<std::string>    arguments_;
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
                            };
                            return visit(extract{}, nm()->get(name));
                        });
    m.def("add",        [nm](std::string name, int    x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, size_t x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, float  x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, double x)    { Value v = x; nm()->add(name, v); });
    m.def("add",        [nm](std::string name, py::array& x)
    {
        Value v;
        if (x.dtype().is(py::dtype::of<float>()))
            v = henson::Array(static_cast<float*>(x.mutable_data()), sizeof(float), x.size(), x.strides(0));
        else if (x.dtype().is(py::dtype::of<double>()))
            v = henson::Array(static_cast<double*>(x.mutable_data()), sizeof(double), x.size(), x.strides(0));
        else if (x.dtype().is(py::dtype::of<int>()))
            v = henson::Array(static_cast<int*>(x.mutable_data()), sizeof(int), x.size(), x.strides(0));
        else if (x.dtype().is(py::dtype::of<long>()))
            v = henson::Array(static_cast<long*>(x.mutable_data()), sizeof(long), x.size(), x.strides(0));
        else
            throw std::runtime_error("Unknown array dtype");

        nm()->add(name, v);
    });

    m.def("create_queue",   [nm](std::string name)          { nm()->create_queue(name); });
    m.def("queue_empty",    [nm](std::string name)          { return nm()->queue_empty(name); });

    m.def("exists",     [nm](std::string name)      { return nm()->exists(name); });
    m.def("clear",      [nm]()                      { nm()->clear(); });
}

#endif
