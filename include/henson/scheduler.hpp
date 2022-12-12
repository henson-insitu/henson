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

#include <henson/logger.hpp>

#include <chaiscript/chaiscript.hpp>
namespace chs = chaiscript;

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
            Job(size_t id_, std::string name_, std::string function_, MemoryBuffer arg_, ProcMap::Vector groups_, int size_):
                id(id_), name(name_), function(function_), arg(arg_), groups(groups_), size(size_)
            {}

            size_t                      id;
            std::string                 name;
            std::string                 function;
            MemoryBuffer                arg;
            ProcMap::Vector             groups;
            size_t                      size;
        };

        struct ActiveJob
        {
            size_t  first, last;
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

                Scheduler(henson::ProcMap* proc_map, int controller_ranks = 1):
                    world_(proc_map->local()), proc_map_(proc_map), controller_ranks_(controller_ranks)
        {
            MPI_Comm_rank(world_, &rank_);
            MPI_Comm_size(world_, &size_);

            // Wait for everyone, then initialize list of available procs
            MPI_Barrier(world_);
            available_procs_.assign(size_, true);
            for (int rk = 0; rk < controller_ranks_; ++rk)
                available_procs_[rk] = false;    // controller is always busy

            // create a local communicator for controller ranks
            MPI_Comm new_comm;
            MPI_Comm_split(world_, is_controller() ? 0 : 1, rank_, &new_comm);
            if (is_controller())
                proc_map_->extend(new_comm, ProcMap::Vector { { "controllers", controller_ranks_ } });
            else
                MPI_Comm_free(&new_comm);
        }

        virtual        ~Scheduler()                { if (is_controller()) proc_map_->pop_back(); }

        int     size() const                { return size_; }
        int     rank() const                { return rank_; }
        int     workers() const             { return size_ - controller_ranks_; }
        bool    job_queue_empty() const     { return jobs_.empty(); }
        bool    is_controller() const       { return rank_ < controller_ranks_; }

        bool    results_empty() const       { return results_.empty(); }
        MemoryBuffer
                pop()                       { auto bv = results_.front(); results_.pop(); return bv; }


        void    schedule(std::string name, std::string function, const MemoryBuffer& arg, ProcMap::Vector groups, int size)
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

        void    listen(std::function<MemoryBuffer(Job&)> runner)
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
                    MPI_Get_count(&job_status, MPI_BYTE, &buffer_size);
                    henson::MemoryBuffer bb;
                    bb.buffer.resize(buffer_size);

                    MPI_Recv(bb.data(), bb.size(), MPI_BYTE, 0, tags::job, world_, MPI_STATUS_IGNORE);

                    size_t first, last;
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

                    MemoryBuffer result = runner(job);

                    if (!result.empty())
                    {
                        log_->debug("Sending back result");
                        henson::MemoryBuffer bb;
                        henson::save(bb, job.id);
                        henson::save(bb, job.name);
                        henson::save(bb, result);
                        MPI_Send(bb.data(), bb.size(), MPI_BYTE, 0, tags::job_finished, world_);
                    } else
                        log_->debug("Result undefined");

                    //chs::Boxed_Value result;
                    //if (job.arg.get_type_info().is_undef())
                    //{
                    //    // call the argument-free version of the function
                    //    auto function = chai_->eval<std::function<chs::Boxed_Value()>>(job.function);
                    //    result = function();
                    //} else
                    //{
                    //    auto function = chai_->eval<std::function<chs::Boxed_Value(chs::Boxed_Value)>>(job.function);
                    //    result = function(job.arg);
                    //}
                    //auto& result_ti = result.get_type_info();
                    //if (!(result_ti.is_undef() || result_ti.is_void() || result_ti.bare_equal_type_info(typeid(bool))))
                    //{
                    //    log_->debug("Sending back result");
                    //    henson::MemoryBuffer bb;
                    //    henson::save(bb, job.id);
                    //    henson::save(bb, job.name);
                    //    henson::save(bb, result);
                    //    MPI_Send(bb.data(), bb.size(), MPI_BYTE, 0, tags::job_finished, world_);
                    //} else
                    //    log_->debug("Result undefined");
                    //log_->debug("Finished job {}", job.name);

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

    protected:
        void    log_job_times()
        {
            for (auto& x : job_times_)
                log_->info("Job {} ({}) time: {}",  x.name, x.id, x.duration);
        }

        size_t  find_available_procs(size_t num_procs)
        {
            size_t procs = 0;
            for(size_t i = 1; i < available_procs_.size(); ++i)
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

            size_t first = find_available_procs(job.size);
            size_t last  = first + job.size - 1;
            log_->debug("Scheduling on {} - {}", first, last);

            henson::MemoryBuffer bb;
            henson::save(bb, first);
            henson::save(bb, last);
            henson::save(bb, job);

            // TODO: replace point-to-point with a broadcast from the leader onward
            double start_time = MPI_Wtime();        // NB: timer starts at the point of sending
            for(size_t i = first; i <= last; ++i)
            {
                MPI_Send(bb.data(), bb.size(), MPI_BYTE, i, tags::job, world_);
                available_procs_[i] = false;
            }

            active_jobs_[job.id] = { first, last, start_time };
        }

        bool    unfinished_jobs()               { return !active_jobs_.empty(); }

        void    signal_stop()
        {
            // TODO: Replace with a reduction
            for(size_t i = 1; i < available_procs_.size(); i++)
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
                MPI_Get_count(&job_status, MPI_BYTE, &buffer_size);
                henson::MemoryBuffer bb;
                bb.buffer.resize(buffer_size);

                int from = job_status.MPI_SOURCE;
                MPI_Recv(bb.data(), bb.size(), MPI_BYTE, from, tags::job_finished, world_, MPI_STATUS_IGNORE);
                double end_time = MPI_Wtime();

                size_t      id;
                std::string name;
                MemoryBuffer result;
                henson::load(bb, id);
                henson::load(bb, name);
                henson::load(bb, result);
                results_.push(result);

                if (active_jobs_.find(id) == active_jobs_.end())
                    log_->error("Received answer from an unknown job: {}", id);
                auto aj = active_jobs_[id];
                active_jobs_.erase(id);
                log_->debug("Received answer from {}; name = {} ({}), procs = {} - {}", from, name, id, aj.first, aj.last);

                for (size_t i = aj.first; i <= aj.last; ++i)
                    available_procs_[i] = true;

                job_times_.push_back(TimeRecord { id, name, end_time - aj.start });
            }
        }

        static void create_group(size_t first, size_t last, MPI_Comm parent_comm, MPI_Group* new_group)
        {
            MPI_Group parent_group;
            MPI_Comm_group(parent_comm, &parent_group);

            std::vector<int> group_array(last - first + 1, -1);
            for(size_t i = 0; i < group_array.size(); ++i)
                group_array[i] = first + i;

            MPI_Group_incl(parent_group, group_array.size(), group_array.data(), new_group);
        }

        MPI_Comm                                            world_;
        int                                                 rank_;
        int                                                 size_;

        std::queue<Job>                                     jobs_;
        std::map<size_t, ActiveJob>                         active_jobs_;
        std::queue<MemoryBuffer>                            results_;
        std::vector<TimeRecord>                             job_times_;
        size_t                                              job_id_ = 0;

        ProcMap*                                            proc_map_;

        int                                                 controller_ranks_;
        std::vector<bool>                                   available_procs_;

        std::shared_ptr<spd::logger>                        log_ = get_or_create_logger();
};

struct ChaiScheduler : public Scheduler {

    ChaiScheduler(chs::ChaiScript* chai, henson::ProcMap* proc_map, int controller_ranks = 1):
        Scheduler(proc_map, controller_ranks), chai_(chai) {}

    chs::ChaiScript*                             chai_;

    chs::Boxed_Value                             pop_chai()
    {
        MemoryBuffer mb = pop();
        chs::Boxed_Value result;
        load(mb, result);
        return result;
    }

    void listen_chai()
    {
        std::function<MemoryBuffer(Job&)> runner = [this](Job& job)
        {
            chs::Boxed_Value bv_result;
            if (job.arg.empty())
            {
                // call the argument-free version of the function
                auto function = chai_->eval<std::function<chs::Boxed_Value()>>(job.function);
                bv_result = function();
            } else
            {
                auto function = chai_->eval<std::function<chs::Boxed_Value(chs::Boxed_Value)>>(job.function);
                chs::Boxed_Value bv_arg;
                job.arg.reset();                     // set position to 0 so that load reads data from the beginning of the buffer
                load(job.arg, bv_arg);
                bv_result = function(bv_arg);
            }

            MemoryBuffer result;
            if (!detail::is_boxed<bool>(bv_result))
                save(result, bv_result);
            return result;
        };
        listen(runner);
    }
};

template<>
struct Serialization<Scheduler::Job>
{
    static void         save(BinaryBuffer& bb, const Scheduler::Job& j)
    {
        henson::save(bb, j.id);
        henson::save(bb, j.name);
        henson::save(bb, j.function);
        henson::save(bb, j.arg);
        henson::save(bb, j.groups);
        henson::save(bb, j.size);
    }

    static void         load(BinaryBuffer& bb, Scheduler::Job& j)
    {
        henson::load(bb, j.id);
        henson::load(bb, j.name);
        henson::load(bb, j.function);
        henson::load(bb, j.arg);
        henson::load(bb, j.groups);
        henson::load(bb, j.size);
    }
};

}

#endif
