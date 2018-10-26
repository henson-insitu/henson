#include <memory>

#include <henson/chai/scheduler.hpp>
#include <henson/scheduler.hpp>
namespace h = henson;

extern std::shared_ptr<h::ProcMap> proc_map;

void chai_scheduler(chaiscript::ChaiScript& chai, int controller_ranks)
{
    // Scheduler
    chai.add(chaiscript::fun([&chai,controller_ranks]()
    {
        return std::make_shared<h::Scheduler>(proc_map->local(), &chai, proc_map.get(), controller_ranks);
    }),                                                                     "Scheduler");

    using BV = chaiscript::Boxed_Value;
    auto clone    = chai.eval<std::function<BV (const BV&)>>("clone");
    auto schedule = [clone](h::Scheduler* s, std::string name, std::string function, chaiscript::Boxed_Value arg, std::map<std::string, chaiscript::Boxed_Value> groups, int size)
    {
        h::ProcMap::Vector groups_vector;

        // divide unused procs between groups of size <= 0
        std::vector<std::string> unspecified;
        int specified = 0;
        for (auto& x : groups)
        {
            int sz = chaiscript::boxed_cast<int>(x.second);
            if (sz <= 0)
                unspecified.push_back(x.first);
            else
                specified += sz;
        }

        int leftover = size - specified;
        int leftover_group_size = size / unspecified.size();
        for (auto& x : groups)
        {
            int sz = chaiscript::boxed_cast<int>(x.second);
            if (sz > 0)
                groups_vector.emplace_back(x.first, sz);
            else if (x.first == unspecified.back())
                groups_vector.emplace_back(x.first, leftover - leftover_group_size * (unspecified.size() - 1));     // in case leftover doesn't divide evenly
            else
                groups_vector.emplace_back(x.first, leftover_group_size);
        }
        s->schedule(name, function, clone(arg), groups_vector, size);
    };
    chai.add(chaiscript::fun(schedule),                                     "schedule");
    chai.add(chaiscript::fun(&h::Scheduler::listen),                        "listen");
    chai.add(chaiscript::fun(&h::Scheduler::size),                          "size");
    chai.add(chaiscript::fun(&h::Scheduler::rank),                          "rank");
    chai.add(chaiscript::fun(&h::Scheduler::workers),                       "workers");
    chai.add(chaiscript::fun(&h::Scheduler::job_queue_empty),               "job_queue_empty");
    chai.add(chaiscript::fun(&h::Scheduler::is_controller),                 "is_controller");
    chai.add(chaiscript::fun(&h::Scheduler::control),                       "control");
    chai.add(chaiscript::fun(&h::Scheduler::results_empty),                 "results_empty");
    chai.add(chaiscript::fun(&h::Scheduler::pop),                           "pop");
    chai.add(chaiscript::fun(&h::Scheduler::finish),                        "finish");
}
