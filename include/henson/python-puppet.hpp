#ifndef HENSON_PYTHON_PUPPET_HPP
#define HENSON_PYTHON_PUPPET_HPP

#include <pybind11/embed.h>
namespace py = pybind11;

#ifdef USE_BOOST
#include <boost/context/all.hpp>
#else
#include <coro.h>
#endif

#include <spdlog/spdlog.h>
namespace spd = spdlog;

namespace henson
{
#ifdef USE_BOOST
namespace bc = boost::context;
#endif

struct PythonPuppet
{
#ifdef USE_BOOST
    typedef             bc::fcontext_t      context_t;
#else
    typedef             coro_context        context_t;
#endif

                        PythonPuppet(const std::string& filename, ProcMap* procmap, NameMap* namemap):
                            filename_(filename), procmap_(procmap), namemap_(namemap)
                        {
                            puppet_name_ = filename_;
#ifdef USE_BOOST
                            stack_ = allocator_.allocate();
                            to_ = bc::make_fcontext(stack_.sp, stack_.size, exec);
#else
                            coro_stack_alloc(&stack_, 8*1024*1024);     // 8MB stack
                            coro_create(&to_, exec, this, stack_.sptr, stack_.ssze);
                            coro_create(&from_, NULL, NULL, NULL, 0);
#endif
                        }

#ifdef USE_BOOST
                        ~PythonPuppet()
                        {
                            if(running_)
                            {
                                signal_stop();
                                proceed();
                            }

                            allocator_.deallocate(stack_);
                        }
    void                proceed()               { start_time_ = get_time(); bc::jump_fcontext(&from_, to_, (intptr_t) this); time_type diff = get_time() - start_time_; total_time_ += diff; }
    void                yield()                 { bc::jump_fcontext(&to_, from_, 0); }

#else
                        ~PythonPuppet()
                        {
                            if(running_)
                            {
                                signal_stop();
                                proceed();
                            }

                            coro_stack_free(&stack_);
                        }
    void                proceed()               { start_time_ = get_time(); coro_transfer(&from_, &to_); time_type diff = get_time() - start_time_; total_time_ += diff; }
    void                yield()                 { coro_transfer(&to_, &from_); }
#endif


    // can't even move a puppet since the addresses of its from_ and to_ fields
    // are stored in the modules (saved via henson_set_context, in the constructor above)
                        PythonPuppet(const PythonPuppet&)   =delete;
                        PythonPuppet(PythonPuppet&&)        =delete;

    PythonPuppet&       operator=(const PythonPuppet&)=delete;
    PythonPuppet&       operator=(PythonPuppet&&)     =delete;

    void                signal_stop()           { stop_ = 1; }

    bool                running() const         { return running_; }
    int                 result() const          { return result_; }

    time_type           total_time() const      { return total_time_; }

#if USE_BOOST
    static void         exec(intptr_t self_)
#else
    static void         exec(void* self_)
#endif
    {
        PythonPuppet* self = (PythonPuppet*) self_;

        while(true)
        {
            py::object scope = py::module::import("__main__").attr("__dict__");
            scope["henson_yield"]           = py::module::import("pyhenson").attr("yield_");
            scope["henson_get_array"]       = py::module::import("pyhenson").attr("get_array");
            scope["henson_get"]             = py::module::import("pyhenson").attr("get");
            scope["henson_add"]             = py::module::import("pyhenson").attr("add");
            scope["henson_create_queue"]    = py::module::import("pyhenson").attr("create_queue");
            scope["henson_queue_empty"]     = py::module::import("pyhenson").attr("queue_empty");
            scope["henson_exists"]          = py::module::import("pyhenson").attr("exists");
            scope["henson_clear"]           = py::module::import("pyhenson").attr("clear");

            // we can have only one PythonPuppet (otherwise scoped_interpreter
            // will complain), so there is only one pyhenson module, with only
            // one self to modify
            py::module::import("pyhenson").attr("self") = reinterpret_cast<intptr_t>(self_);

            self->running_ = true;
            self->start_time_ = get_time();
            py::eval_file(self->filename_, scope);
            self->running_ = false;
            self->yield();      // the time for the final portion will get recorded thanks to this call
        }
    }

#ifdef USE_BOOST
    bc::stack_context   stack_;
    bc::fixedsize_stack allocator_;
#else
    coro_stack          stack_;
#endif

    py::scoped_interpreter  guard_;

    std::string         filename_;
    ProcMap*            procmap_;
    NameMap*            namemap_;

    context_t           from_, to_;
    bool                running_ = false;
    int                 stop_ = 0;
    int                 result_ = -1;

    time_type           start_time_;
    time_type           total_time_ = 0;
    std::string         puppet_name_;

    std::shared_ptr<spd::logger> log_ = spd::get("henson");
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
