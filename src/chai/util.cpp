#include <iostream>

#include <henson/chai/util.hpp>
#include <henson/time.hpp>
namespace h = henson;

extern bool abort_on_segfault_;

void chai_util(chaiscript::ChaiScript& chai)
{
    chai.add(chaiscript::fun([](int secs) { sleep(secs); }),                "sleep");
    chai.add(chaiscript::fun([]() { std::cout << std::flush; }),            "flush");
    chai.add(chaiscript::fun(&h::get_time),                                 "time");
    chai.add(chaiscript::fun(&h::clock_to_string),                          "clock_to_string");
    chai.add(chaiscript::fun([](bool abort_on_segfault)
                             { abort_on_segfault_ = abort_on_segfault; }),  "abort_on_segfault");
    chai.add(chaiscript::fun([](std::string dir)
                             { return chdir(dir.c_str()); }),               "chdir");
}
