#include <iostream>
#include <memory>

#include <henson/chai/scheduler.hpp>
#include <henson/scheduler.hpp>
namespace h = henson;
namespace chs = chaiscript;

extern std::shared_ptr<h::ProcMap> proc_map;

void chai_scheduler(chs::ChaiScript& chai, int controller_ranks)
{
    // Scheduler
    chai.add(chs::user_type<h::Scheduler>(), "BaseScheduler");
    chai.add(chs::user_type<h::ChaiScheduler>(), "Scheduler");
    chai.add(chs::base_class<h::Scheduler, h::ChaiScheduler>());

    chai.add(chs::fun([&chai,controller_ranks]()
    {
        return std::make_shared<h::ChaiScheduler>(&chai, proc_map.get(), controller_ranks);
    }),                                                                     "Scheduler");

    using BV = chs::Boxed_Value;
    auto clone    = chai.eval<std::function<BV (const BV&)>>("clone");
    auto schedule = [clone](h::ChaiScheduler* s, std::string name, std::string function, chs::Boxed_Value arg, std::map<std::string, chs::Boxed_Value> groups, int size)
    {
        h::ProcMap::Vector groups_vector;

        // divide unused procs between groups of size <= 0
        std::vector<std::string> unspecified;
        int specified = 0;
        for (auto& x : groups)
        {
            int sz = chs::boxed_cast<int>(x.second);
            if (sz <= 0)
                unspecified.push_back(x.first);
            else
                specified += sz;
        }

        int leftover = size - specified;
        int leftover_group_size = size / unspecified.size();
        for (auto& x : groups)
        {
            int sz = chs::boxed_cast<int>(x.second);
            if (sz > 0)
                groups_vector.emplace_back(x.first, sz);
            else if (x.first == unspecified.back())
                groups_vector.emplace_back(x.first, leftover - leftover_group_size * (unspecified.size() - 1));     // in case leftover doesn't divide evenly
            else
                groups_vector.emplace_back(x.first, leftover_group_size);
        }
        h::MemoryBuffer mb_arg;
        h::save(mb_arg, clone(arg));
        s->schedule(name, function, mb_arg, groups_vector, size);
    };
    chai.add(chs::fun(schedule),                                     "schedule");

    chai.add(chs::fun(&h::Scheduler::size),                          "size");
    chai.add(chs::fun(&h::Scheduler::rank),                          "rank");
    chai.add(chs::fun(&h::Scheduler::workers),                       "workers");
    chai.add(chs::fun(&h::Scheduler::job_queue_empty),               "job_queue_empty");
    chai.add(chs::fun(&h::Scheduler::is_controller),                 "is_controller");
    chai.add(chs::fun(&h::Scheduler::control),                       "control");
    chai.add(chs::fun(&h::Scheduler::results_empty),                 "results_empty");
    chai.add(chs::fun(&h::Scheduler::finish),                        "finish");

    chai.add(chs::fun(&h::ChaiScheduler::listen_chai),               "listen");
    chai.add(chs::fun(&h::ChaiScheduler::pop_chai),                  "pop");
}
