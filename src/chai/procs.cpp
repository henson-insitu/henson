#include <memory>

#include <henson/chai/procs.hpp>
#include <henson/procs.hpp>
namespace h = henson;

extern std::shared_ptr<h::ProcMap> proc_map;

void chai_procs(chaiscript::ChaiScript& chai)
{
    // ProcMap
    chai.add(chaiscript::fun([]() { return proc_map; }),                    "ProcMap");
    chai.add(chaiscript::fun(&h::ProcMap::group),                           "group");
    chai.add(chaiscript::fun(&h::ProcMap::color),                           "color");
    chai.add(chaiscript::fun(&h::ProcMap::world_rank),                      "world_rank");
    chai.add(chaiscript::fun(&h::ProcMap::local_rank),                      "local_rank");
    chai.add(chaiscript::fun([](h::ProcMap* pm, std::string to)
                             { pm->intercomm(to); }),                       "intercomm");
    chai.add(chaiscript::fun(&h::ProcMap::size),                            "size");
}
