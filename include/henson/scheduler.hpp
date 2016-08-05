#ifndef HENSON_SCHEDULER_HPP
#define HENSON_SCHEDULER_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <exception>

#include <mpi.h>

#include <spdlog/spdlog.h>
namespace spd = spdlog;

#include <chaiscript/chaiscript.hpp>

#include <henson/procs.hpp>
#include <henson/serialization.hpp>
#include <henson/chai-serialization.hpp>
#include <henson/mpi-noncollective-comm.h>


namespace henson
{

// TODO: generalize data exchange. Need to add serialization of at least some
//       range of values returned by the function. It would also be good to
//       have a way to pass arbitrary buffers with data between jobs and
//       scheduler
class Scheduler
{
    public:
        struct Job
        {
            Job() = default;
            Job(size_t id_, std::string name_, std::string function_, chaiscript::Boxed_Value arg_, ProcMap::Vector groups_, int size_):
                id(id_), name(name_), function(function_), groups(groups_), size(size_)
            {
                henson::save(arg_serialized, arg_);
            }

            size_t                      id;
            std::string                 name;
            std::string                 function;
            MemoryBuffer                arg_serialized;
            ProcMap::Vector             groups;
            int                         size;
        };

        struct ActiveJob
        {
            int     first, last;
            double  start;
        };

        struct TimeRecord
        {
            size_t          id;
            std::string     name;
            double          duration;
        };

        struct tags
        {
            enum
            {
                job = 1, stop, job_finished, intercomm_find,
                noncollective_comm_split = 77
            };
        };

                Scheduler(MPI_Comm world,  chaiscript::ChaiScript* chai, henson::ProcMap* proc_map):
                    world_(world), chai_(chai), proc_map_(proc_map)
        {
            MPI_Comm_rank(world_, &rank_);
            MPI_Comm_size(world_, &size_);

            // Wait for everyone, then initialize list of available procs
            MPI_Barrier(world_);
            available_procs_.assign(size_, true);
            available_procs_[0] = false;    // controller is always busy
        }

        int     size() const                { return size_; }
        int     rank() const                { return rank_; }
        int     workers() const             { return size_ - 1; }
        bool    job_queue_empty() const     { return jobs_.empty(); }
        bool    is_controller() const       { return rank_ == 0; }

        bool    results_empty() const       { return results_.empty(); }
        chaiscript::Boxed_Value
                pop()                       { auto bv = results_.front(); results_.pop(); return bv; }


        void    schedule(std::string name, std::string function, chaiscript::Boxed_Value arg, ProcMap::Vector groups, int size)
        {
            jobs_.emplace(job_id_++, name, function, arg, groups, size);
        }

        bool    control()
        {
            check_for_complete_jobs();

            // while jobs and available processes, schedule jobs
            // TODO: this logic should be updated to look through all jobs for any that can be scheduled
            while(!jobs_.empty() && find_available_procs(jobs_.front().size) < available_procs_.size())
                run_job();

            return unfinished_jobs() || !jobs_.empty();
        }

        void    listen()
        {
            while (true)
            {
                log_->debug("Waiting for a job");
                MPI_Status job_status;
                MPI_Probe(0, MPI_ANY_TAG, world_, &job_status);

                if(job_status.MPI_TAG == tags::stop)
                {
                    log_->debug("Received job stop signal");
                    MPI_Recv(nullptr, 0, MPI_INT, 0, tags::stop, world_, MPI_STATUS_IGNORE);
                    break;
                }
                else if(job_status.MPI_TAG == tags::job)
                {
                    int buffer_size;
                    MPI_Get_count(&job_status, MPI_CHAR, &buffer_size);
                    henson::MemoryBuffer bb;
                    bb.buffer.resize(buffer_size);

                    MPI_Recv(bb.data(), bb.size(), MPI_CHAR, 0, tags::job, world_, MPI_STATUS_IGNORE);

                    int first, last;
                    Job job;
                    henson::load(bb, first);
                    henson::load(bb, last);
                    henson::load(bb, job);
                    log_->debug("Got job assignment: {} ({} - {})", job.name, first, last);

                    // Create job communicator
                    MPI_Group job_group;
                    MPI_Comm  job_world;
                    create_group(first, last, world_, &job_group);
                    non_collective_comm_create(job_group, world_, tags::noncollective_comm_split, &job_world);

                    proc_map_->extend(job_world, job.groups);

                    chaiscript::Boxed_Value arg;
                    chaiscript::Boxed_Value result;
                    job.arg_serialized.reset();
                    henson::load(job.arg_serialized, arg);
                    if (arg.get_type_info().is_undef())
                    {
                        // call the argument-free version of the function
                        auto function = chai_->eval<std::function<chaiscript::Boxed_Value()>>(job.function);
                        result = function();
                    } else
                    {
                        auto function = chai_->eval<std::function<chaiscript::Boxed_Value(chaiscript::Boxed_Value)>>(job.function);
                        result = function(arg);
                    }
                    auto& result_ti = result.get_type_info();
                    if (!(result_ti.is_undef() || result_ti.is_void() || result_ti.bare_equal_type_info(typeid(bool))))
                    {
                        log_->debug("Sending back result");
                        henson::MemoryBuffer bb;
                        henson::save(bb, job.id);
                        henson::save(bb, job.name);
                        henson::save(bb, result);
                        MPI_Send(bb.data(), bb.size(), MPI_CHAR, 0, tags::job_finished, world_);
                    } else
                        log_->debug("Result undefined");
                    log_->debug("Finished job {}", job.name);

                    proc_map_->pop_back();
                }
            }
        }

        void    finish()
        {
            log_->debug("Finishing scheduler controller");
            while (unfinished_jobs())
                check_for_complete_jobs();

            log_job_times();
            signal_stop();
        }

    private:
        void    log_job_times()
        {
            for (auto& x : job_times_)
                log_->info("Job {} ({}) time: {}",  x.name, x.id, x.duration);
        }

        int     find_available_procs(int num_procs)
        {
            int procs = 0;
            for(int i = 1; i < available_procs_.size(); ++i)
            {
                if (available_procs_[i])
                    procs++;
                else
                    procs = 0;

                if (procs == num_procs)
                    return i - (procs - 1);
            }
            return available_procs_.size();
        }

        void    run_job()
        {
            Job job = jobs_.front();
            jobs_.pop();

            int first = find_available_procs(job.size);
            int last  = first + job.size - 1;
            log_->debug("Scheduling on {} - {}", first, last);

            henson::MemoryBuffer bb;
            henson::save(bb, first);
            henson::save(bb, last);
            henson::save(bb, job);

            // TODO: replace point-to-point with a broadcast from the leader onward
            double start_time = MPI_Wtime();        // NB: timer starts at the point of sending
            for(int i = first; i <= last; ++i)
            {
                MPI_Send(bb.data(), bb.size(), MPI_CHAR, i, tags::job, world_);
                available_procs_[i] = false;
            }

            // TODO: this creates a problem if job names collide; might not want to do this based on names
            active_jobs_[job.id] = { first, last, start_time };
        }

        bool    unfinished_jobs()               { return !active_jobs_.empty(); }

        void    signal_stop()
        {
            // TODO: Replace with a reduction
            for(int i = 1; i < available_procs_.size(); i++)
                MPI_Send(nullptr, 0, MPI_INT, i, tags::stop, world_);
        }

        int     iprobe(int source, int tag, MPI_Status& job_status)
        {
            int flag;
            MPI_Iprobe(source, tag, world_, &flag, &job_status);
            return flag;
        }

        void    check_for_complete_jobs()
        {
            MPI_Status job_status;
            while (iprobe(MPI_ANY_SOURCE, tags::job_finished, job_status))
            {
                int buffer_size;
                MPI_Get_count(&job_status, MPI_CHAR, &buffer_size);
                henson::MemoryBuffer bb;
                bb.buffer.resize(buffer_size);

                int from = job_status.MPI_SOURCE;
                MPI_Recv(bb.data(), bb.size(), MPI_CHAR, from, tags::job_finished, world_, MPI_STATUS_IGNORE);
                double end_time = MPI_Wtime();

                size_t      id;
                std::string name;
                chaiscript::Boxed_Value result;
                henson::load(bb, id);
                henson::load(bb, name);
                henson::load(bb, result);
                results_.push(result);

                if (active_jobs_.find(id) == active_jobs_.end())
                    log_->error("Received answer from an unknown job: {}", id);
                auto aj = active_jobs_[id];
                active_jobs_.erase(id);
                log_->debug("Received answer from {}; name = {} ({}), procs = {} - {}", from, name, id, aj.first, aj.last);

                for (int i = aj.first; i <= aj.last; ++i)
                    available_procs_[i] = true;

                job_times_.push_back(TimeRecord { id, name, end_time - aj.start });
            }
        }

        static void create_group(int first, int last, MPI_Comm parent_comm, MPI_Group* new_group)
        {
            MPI_Group parent_group;
            MPI_Comm_group(parent_comm, &parent_group);

            std::vector<int> group_array(last - first + 1, -1);
            for(int i = 0; i < group_array.size(); ++i)
                group_array[i] = first + i;

            MPI_Group_incl(parent_group, group_array.size(), group_array.data(), new_group);
        }

        MPI_Comm                                            world_;
        int                                                 rank_;
        int                                                 size_;

        std::vector<bool>                                   available_procs_;

        std::queue<Job>                                     jobs_;
        std::map<size_t, ActiveJob>                         active_jobs_;
        std::queue<chaiscript::Boxed_Value>                 results_;
        std::vector<TimeRecord>                             job_times_;
        size_t                                              job_id_ = 0;

        chaiscript::ChaiScript*                             chai_;
        ProcMap*                                            proc_map_;

        std::shared_ptr<spd::logger>                        log_ = spd::get("henson");
};

template<>
struct Serialization<Scheduler::Job>
{
    static void         save(BinaryBuffer& bb, const Scheduler::Job& j)
    {
        henson::save(bb, j.id);
        henson::save(bb, j.name);
        henson::save(bb, j.function);
        henson::save(bb, j.arg_serialized);
        henson::save(bb, j.groups);
        henson::save(bb, j.size);
    }

    static void         load(BinaryBuffer& bb, Scheduler::Job& j)
    {
        henson::load(bb, j.id);
        henson::load(bb, j.name);
        henson::load(bb, j.function);
        henson::load(bb, j.arg_serialized);
        henson::load(bb, j.groups);
        henson::load(bb, j.size);
    }
};

}

#endif
