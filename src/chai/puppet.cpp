#include <henson/chai/puppet.hpp>

#include <henson/command-line.hpp>
#include <henson/puppet.hpp>
#ifdef HENSON_PYTHON
#include <henson/python-puppet.hpp>
#endif
namespace h = henson;

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
namespace spd = spdlog;
extern std::string active_puppet;
extern std::shared_ptr<spd::logger> logger;

void chai_puppet(chaiscript::ChaiScript& chai, henson::NameMap& namemap, std::string script_prefix)
{
    // BaseCoroutine
    chai.add(chaiscript::user_type<h::BaseCoroutine>(), "BaseCoroutine");
    chai.add(chaiscript::fun(&h::BaseCoroutine::running),      "running");
    chai.add(chaiscript::fun(&h::BaseCoroutine::signal_stop),  "signal_stop");
    chai.add(chaiscript::fun(&h::BaseCoroutine::total_time),   "total_time");
    chai.add(chaiscript::fun(&h::BaseCoroutine::name),         "name");

    // Puppet
    chai.add(chaiscript::base_class<h::BaseCoroutine, h::Puppet>());
    chai.add(chaiscript::user_type<h::Puppet>(),        "Puppet");
    chai.add(chaiscript::fun([](h::Puppet& puppet)
    {
        active_puppet = puppet.name();
        logger->debug("Proceeding with {}", puppet.name());
        puppet.proceed();
        return puppet.running();
    }), "proceed");

    chai.add(chaiscript::fun([&namemap,script_prefix](std::string cmd_line_str, henson::ProcMap* pm)
    {
        auto cmd_line = h::CommandLine(cmd_line_str);
        return std::make_shared<h::Puppet>(cmd_line.executable(script_prefix),
                                           cmd_line.argv.size(),
                                           &cmd_line.argv[0],
                                           pm,
                                           &namemap);
    }), "load");

#ifdef HENSON_PYTHON
    // PythonPuppet
    chai.add(chaiscript::base_class<h::BaseCoroutine, h::PythonPuppet>());
    chai.add(chaiscript::user_type<h::PythonPuppet>(),  "PythonPuppet");
    chai.add(chaiscript::fun([](h::PythonPuppet& puppet)
    {
        active_puppet = puppet.name();
        logger->debug("Proceeding with {}", puppet.name());
        puppet.proceed();
        return puppet.running();
    }), "proceed");
    chai.add(chaiscript::fun([&namemap,script_prefix](std::string python_script, henson::ProcMap* pm)
    {
        if (python_script[0] != '/' && python_script[0] != '~')
            python_script = script_prefix + python_script;
        return std::make_shared<h::PythonPuppet>(python_script, pm, &namemap);
    }), "python");
#endif
}
