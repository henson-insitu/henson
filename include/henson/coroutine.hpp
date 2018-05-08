#ifndef HENSON_COROUTINE_HPP
#define HENSON_COROUTINE_HPP

#include <string>

#ifdef USE_BOOST
#include <boost/context/all.hpp>
#else
#include <coro.h>
#endif

#include <henson/time.hpp>

namespace henson
{

#ifdef USE_BOOST
namespace bc = boost::context;
#endif

struct BaseCoroutine
{
                        BaseCoroutine(const std::string& name):
                            name_(name)         {}

    void                signal_stop()           { stop_ = 1; }

    bool                running() const         { return running_; }
    int                 result() const          { return result_; }

    time_type           total_time() const      { return total_time_; }

    const std::string&  name() const            { return name_; }


    std::string         name_;

    bool                running_ = false;
    int                 stop_ = 0;
    int                 result_ = -1;

    time_type           start_time_;
    time_type           total_time_ = 0;
};

template<class Derived>
struct Coroutine: public BaseCoroutine
{
#ifdef USE_BOOST
    using context_t = bc::fcontext_t;
    using exec_t    = intptr_t;
#else
    using context_t = coro_context;
    using exec_t    = void*;
#endif

                        Coroutine(const std::string& name):
                            BaseCoroutine(name)
                        {
#ifdef USE_BOOST
                            stack_ = allocator_.allocate();
                            to_ = bc::make_fcontext(stack_.sp, stack_.size, Derived::exec);
#else
                            coro_stack_alloc(&stack_, 8*1024*1024);     // 8MB stack
                            coro_create(&to_, Derived::exec, this, stack_.sptr, stack_.ssze);
                            coro_create(&from_, NULL, NULL, NULL, 0);
#endif
                        }

                        ~Coroutine()
                        {
#ifdef USE_BOOST
                            allocator_.deallocate(stack_);
#else
                            coro_stack_free(&stack_);
#endif
                        }

    void                proceed()
    {
        start_time_ = get_time();
#ifdef USE_BOOST
        bc::jump_fcontext(&from_, to_, (intptr_t) this);
#else
        coro_transfer(&from_, &to_);
#endif
        time_type diff = get_time() - start_time_;
        total_time_ += diff;
    }

    void                yield()
    {
#ifdef USE_BOOST
        bc::jump_fcontext(&to_, from_, 0);
#else
        coro_transfer(&to_, &from_);
#endif
    }

    // can't even move a puppet since the addresses of its from_ and to_ fields
    // are stored in the modules (saved via henson_set_context, in the constructor above)
                        Coroutine(const Coroutine&)     =delete;
                        Coroutine(Coroutine&&)          =delete;

    Coroutine&          operator=(const Coroutine&)     =delete;
    Coroutine&          operator=(Coroutine&&)          =delete;

#ifdef USE_BOOST
    bc::stack_context   stack_;
    bc::fixedsize_stack allocator_;
#else
    coro_stack          stack_;
#endif

    context_t           from_, to_;
};

}

#endif
