#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unistd.h>

#include <mpi.h>

#include <opts/opts.h>
#include <format.h>
#include <spdlog/spdlog.h>
namespace spd = spdlog;
std::shared_ptr<spd::logger> logger;

#include <henson/puppet.hpp>
#include <henson/data.hpp>
//#include <henson/data.h>
#include <henson/procs.hpp>
//Henson scheduler has the MPI_Tags
#include <henson/scheduler.hpp>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>
namespace h = henson;

// http://stackoverflow.com/a/1486931/44738
#define UNUSED(expr) do { (void)(expr); } while (0)

std::string
clock_to_seconds(h::time_type t)
{
    return fmt::format("{}.{:02}", t / 1000000000, (t % 1000000000 / 10000000));
}

std::string
clock_output(h::time_type t)
{
    return fmt::format("{} ({})", h::clock_to_string(t), clock_to_seconds(t));
}


//Added this type to interface with Chai better
typedef     std::shared_ptr<h::Puppet>                  PuppetSharedPtr;
typedef     std::map<std::string, PuppetSharedPtr>      Puppets;


int count_argc(const std::string& argv_string)
{
    int count = 0;
    int index = 0;
    while (argv_string[index])
    {
      while (isspace(argv_string[index])) index++;
      while (argv_string[index] && !isspace(argv_string[index])) index++;
      count++;
    }
    return count;
}


bool convert_string_to_argv(const std::string& string_arg, int argc, char** argv)
{
    if (!argv || argc == 0 || string_arg.empty()) return false;

    char * temp = new char [ string_arg.length() + 1 ];
    strcpy(temp, string_arg.c_str());

    int begin, end;
    begin = end = 0;

    for(int i  = 0; i < argc; ++i)
    {

      while(isspace(temp[begin])) begin++;
      end = begin;
      while(temp[end] && !isspace(temp[end]))  end++;

      if(end < begin) return false;
      int size = end - begin;

      argv[i] = new char[size + 1];

      memcpy(argv[i], &temp[begin], size);
      argv[i][size] = '\0';

      begin = end + 1;

    }
    return true;

}

//Need to parse string args into legitamate arguments
bool get_args(const std::string& arg_string, int& argc, char**& argv)
{
    argc = count_argc(arg_string);

    argv = new char*[argc];

    return convert_string_to_argv(arg_string, argc, argv);
}


int main(int argc, char *argv[])
{
    h::time_type start_time = h::get_time();

    MPI_Init(&argc, &argv);

    MPI_Comm world = MPI_COMM_WORLD;

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    std::vector<std::string>    procs_sizes;
    std::string                 log_level = "info";
    using namespace opts;
    Options ops(argc, argv);
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
        >> Option('l', "log",   log_level,   "log level to use")
    ;

    bool show_sizes = ops >> Present('s', "show-sizes", "show group sizes");
    bool verbose    = ops >> Present('v', "verbose",    "verbose output");
    bool times      = ops >> Present('t', "show-times", "show time spent in each puppet");
    bool every_iteration = ops >> Present("every-iteration", "report times at every iteration");

    logger = spd::stderr_logger_st("console");
    logger->set_level(spd::level::off);
    int lvl;
    for (lvl = spd::level::trace; lvl < spd::level::off; ++lvl)
        if (spd::level::level_names[lvl] == log_level)
            break;
    if (verbose || rank == 0)
        logger->set_level(static_cast<spd::level::level_enum>(lvl));

    logger->set_pattern(fmt::format("[{}]: [%Y-%m-%d %H:%M:%S.%e] [%l] %v", rank));

    if (verbose || rank == 0)
        logger->info("henson started; total processes = {}", size);

    std::string script_fn;
    if (  ops >> Present('h', "help", "show help") ||
        !(ops >> PosOption(script_fn)))
    {
        if (rank == 0)
        {
            fmt::print("Usage: {} SCRIPT [-p group=SIZE]* [variable=value]*\n\n", argv[0]);
            fmt::print("Execute SCRIPT. procs are the names of execution groups in the script.\n");
            fmt::print("Leftover processors get split evenly among the execution groups in the SCRIPT\n");
            fmt::print("but not specified in the procs list.\n\n");
            fmt::print("{}", ops);
        }
        return 1;
    }

    //Procs = processors
    // parse the procs
    henson::NameMap                   namemap;
    //Puppets                           puppets;


    ProcMapSharedPtr proc_map = std::make_shared<henson::ProcMap>(world, procs_sizes, size);

    // convert script into puppets
    // TODO: technically, we don't need to load puppets that we don't need, but I'm guessing it's a small overhead


    //Chai-stuff goes here
    chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());
    //chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());
    chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<double>>("VecDouble"));
    chai.add(chaiscript::bootstrap::standard_library::pair_type<henson::Scheduler::StackElement>("StackElement"));
    chai.add(chaiscript::user_type<henson::Puppet>(), "Puppet");
    chai.add(chaiscript::fun(&henson::Puppet::proceed), "proceed");
    chai.add(chaiscript::fun(&henson::Puppet::running), "running");
    chai.add(chaiscript::fun(&henson::Puppet::signal_stop), "signal_stop");

    chai.add(chaiscript::fun([&namemap] () { return &namemap; }), "NameMap");
    //Probably should figure out what to do if something isn't in the map
    chai.add(chaiscript::fun([](henson::NameMap* namemap, std::string name)
                                {
                                    henson::DataType * the_value = namemap->get(name);
                                    if(henson::Value<int> * temp = dynamic_cast< henson::Value<int> * >(the_value))
                                        return chaiscript::Boxed_Value(temp->value);
                                    else if(henson::Value<double> * temp = dynamic_cast< henson::Value<double> * >(the_value))
                                        return chaiscript::Boxed_Value(temp->value);
                                    else if(henson::Value<float> * temp = dynamic_cast< henson::Value<float> * >(the_value))
                                        return chaiscript::Boxed_Value(temp->value);
                                    else if(henson::Value<size_t> * temp = dynamic_cast< henson::Value<size_t> * >(the_value))
                                        return chaiscript::Boxed_Value(temp->value);
                                    else if(henson::Value<void *> * temp = dynamic_cast< henson::Value<void *> * >(the_value))
                                        return chaiscript::Boxed_Value((intptr_t)temp->value);
                                    else
                                    {
                                        throw std::runtime_error("Error: namemap value (" + name + ") was not an accepted type, or may not exist");
                                        return chaiscript::Boxed_Value();
                                    }

                                }), "get");

    chai.add(chaiscript::fun([&chai](chaiscript::Boxed_Value& bv)
                                -> std::vector<double>* {
                                    void * temp = (void *)(chai.boxed_cast<intptr_t>(bv));
                                   //std::cout << "Void * value: " << temp << std::endl;
                                    std::vector<double>* v = (std::vector<double> *)temp;
                                    return v;
                                }), "convert_to_vec_double");

//    chai.add(chaiscript::fun(henson_load_int), "henson_load_int");

    chai.add(chaiscript::fun([&proc_map] () { 
                                                henson::ProcMap * lowest_procmap = proc_map.get();
                                                while(lowest_procmap->get_child()) lowest_procmap = lowest_procmap->get_child();
                                                return lowest_procmap; }), "ProcMap");

    chai.add(chaiscript::fun(&henson::ProcMap::isInGroup), "isInGroup");
    chai.add(chaiscript::fun(&henson::ProcMap::get_local_rank), "get_local_rank");
    chai.add(chaiscript::fun(&henson::ProcMap::get_job_rank), "get_job_rank");
    chai.add(chaiscript::fun(&henson::ProcMap::color), "color");
    chai.add(chaiscript::fun(&henson::ProcMap::get_job_name), "get_job_name");
    chai.add(chaiscript::fun([&proc_map] (const std::string& to) {
                                                                        henson::ProcMap * lowest_procmap = proc_map.get();
                                                                        while(lowest_procmap->get_child()) lowest_procmap = lowest_procmap->get_child();
                                                                        lowest_procmap->intercomm(to);
                                                                 }), "henson_get_intercomm");

    //chai.add(chaiscript::user_type<henson::Scheduler>(), "Scheduler");
    chai.add(chaiscript::fun([&verbose, &chai, &proc_map] () { return std::make_shared<henson::Scheduler>(verbose, proc_map->local(), &chai, proc_map.get()); }), "Scheduler");
    chai.add(chaiscript::fun(&henson::Scheduler::schedule_job), "schedule_job");
   // chai.add(chaiscript::fun(&henson::Scheduler::schedule_loop), "schedule_loop");
    chai.add(chaiscript::fun(&henson::Scheduler::listen), "listen");
    chai.add(chaiscript::fun(&henson::Scheduler::get_size), "get_size");
    chai.add(chaiscript::fun(&henson::Scheduler::get_schedule_rank), "get_schedule_rank");
    chai.add(chaiscript::fun(&henson::Scheduler::is_active), "is_active");
    chai.add(chaiscript::fun(&henson::Scheduler::is_job_queue_empty), "is_job_queue_empty");
    //chai.add(chaiscript::fun(&henson::Scheduler::run_next_job), "run_next_job");
    chai.add(chaiscript::fun(&henson::Scheduler::is_controller), "is_controller");
    chai.add(chaiscript::fun(&henson::Scheduler::control), "control");
    chai.add(chaiscript::fun(&henson::Scheduler::check_for_complete_jobs), "check_for_complete_jobs");
    chai.add(chaiscript::fun(&henson::Scheduler::is_stack_empty), "is_stack_empty");
    chai.add(chaiscript::fun(&henson::Scheduler::next_on_stack), "next_on_stack");
    chai.add(chaiscript::fun(&henson::Scheduler::pop_stack), "pop_stack");
    chai.add(chaiscript::fun(&henson::Scheduler::finish), "finish");


    chai.add(chaiscript::fun([](int secs) { sleep(secs); }), "sleep");
    chai.add(chaiscript::fun([]() { std::cout << std::flush; }), "flush");


    //chai.add(chaiscript::fun(&henson::Scheduler::check_for_complete_jobs), "check_for_complete_jobs");
    //chai.add(chaiscript::fun(&henson::Scheduler::has_unfinished_jobs), "has_unfinished_jobs");
    //chai.add(chaiscript::fun(&henson::Scheduler::send_stop_sched_signal), "send_stop_sched_signal");

    

    chai.add(chaiscript::fun([&](std::string name, std::string location, std::string args, henson::ProcMap* pm)
    {

        int argc = 0;
        char ** argv = NULL;

        std::string prefix = location;
        if (prefix[0] != '/' && prefix[0] != '~')
            prefix = "./" + prefix;
        //prefix = prefix.substr(0, prefix.rfind('/') + 1);

        // parse and process command variables
        std::string exec = prefix; 
        std::string cmdline = exec + " " + args;

        if(!get_args(cmdline, argc, argv))
        {

          //logger->error("Couldn't read args: {}", cmdline);
          std::string error = "Couldn't read cmdline args for puppet: " + name;
          throw std::runtime_error(error);
        }
        logger->debug("Trying to load puppet {}", name);

        PuppetSharedPtr temp_ptr = PuppetSharedPtr(new henson::Puppet(exec,
                                                            argc,
                                                            &argv[0],
                                                            pm,
                                                            &namemap));

        //std::cout << "Returning puppet, successfully loaded!" << std::endl;
        return temp_ptr;

    }), "load");

    //chai.add(chaiscript::fun([&interface] () { interface.print_procmap_group_info(); }), "print_procmap_group_info");

    if(rank == 0)
    {
        //std::cout << "About to send the scripts!" << std::endl;
        std::ifstream script_file(script_fn, std::ios::in|std::ios::binary|std::ios::ate);
        if( script_file.is_open())
        {
            int file_size = script_file.tellg();
            std::vector<char> buffered_in(file_size);
            script_file.seekg(0, std::ios::beg);
            script_file.read(buffered_in.data(), file_size);
            script_file.close();

            //Broadcast the file size then the file itself
            MPI_Bcast(&file_size, 1, MPI_INT, 0, world);

            MPI_Bcast(buffered_in.data(), file_size, MPI_CHAR, 0, world);

            std::string new_string(buffered_in.begin(), buffered_in.end());

            //std::cout << "About to eval the script: " << new_string << "after sending!" << std::endl;
            chai.eval(new_string);
        }
        else
        {
            std::cout << "Error: unable to open file " << script_fn << std::endl;
        }
    }
    else
    {

        //std::cout << "About to recv a script!" << std::endl;
        int file_size = 0;
        MPI_Bcast(&file_size, 1, MPI_INT, 0, world);

        std::vector<char> buffered_in(file_size);

        MPI_Bcast(buffered_in.data(), file_size, MPI_CHAR, 0, world);

        //std::cout << "Successfully received trying to eval now!" << std::endl;
        std::string new_string(buffered_in.begin(), buffered_in.end());
        chai.eval(new_string);
    }

    //std::cout << "On the other side of the eval!" << std::endl;


    //std::vector<char> buffered_in(script_fn.begin(), script_fn.end());
    //std::string new_string(buffered_in.begin(), buffered_in.end());
    //chai.eval(new_string);
    //chai.eval_file(script_fn);

    /*chai.eval(R"(
            var name_map = NameMap()
            "
            */

    

    //chai.eval("test_fun(1, 2, 3)");
    /*chai.add(chaiscript::fun(&BaseClass::doSomething), "doSomething");
    chai.add(chaiscript::fun(&BaseClass::setValue), "setValue");
    chai.add(chaiscript::fun(&BaseClass::getValue), "getValue");
    chai.add(chaiscript::constructor<ChaiScriptDerived (const std::vector<chaiscript::Boxed_Value> &)>(), "ChaiScriptDerived");
    chai.add(chaiscript::base_class<BaseClass, ChaiScriptDerived>());
    chai.add(chaiscript::user_type<BaseClass>(), "BaseClass");
    chai.add(chaiscript::user_type<ChaiScriptDerived>(), "ChaiScriptDerived");
    */


    // Timer reporter
    //h::time_type initialization_time = h::get_time() - start_time;


    logger->info("henson done");

    MPI_Finalize();
}
