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
#include <chaiscript/chaiscript_stdlib.hpp>
#include <henson/procs.hpp>
#include <henson/mpi-noncollective-comm.h>


#define INIT_TAG            1
#define JOB_TAG             2
#define STOP_TAG            3
#define JOB_FINISHED_TAG    4
#define SCRIPT_TAG          5
#define INTERCOMM_FIND_TAG  6
#define NONCOLLECTIVE_TAG   77

namespace henson
{

//typedef     std::shared_ptr<chaiscript::ChaiScript>     ChaiScriptSharedPtr;

struct HensonJob
{
    HensonJob(){}
    HensonJob (std::string j_name, std::string func, std::map<std::string, int> the_groups, int num_p) :
               job_name(j_name), function(func), groups(the_groups), num_procs(num_p)  {}

    std::string job_name;
    std::string function;
    std::map<std::string, int> groups;
    int num_procs;
};

class Scheduler
{

    public:

        typedef std::pair<std::string, double> StackElement;

        Scheduler(MPI_Comm world,  chaiscript::ChaiScript* chai, henson::ProcMap * pm): world_(world), chai_(chai), proc_map_(pm),
                                                                                        is_active_(true), num_consecutive_procs_(0), MAX_JOBS_(5000)

        {
            MPI_Comm_rank(world_, &rank_);
            MPI_Comm_size(world_, &size_);

            //Wait for everyone to say that they are ready, then initialize list of available procs
            if(rank_ == 0)
            {
                //This is the master scheduler
                //Get a ready and waiting for jobs from everyone

                int count = 0;
                available_procs_.assign(size_ - 1, 0);
                while(count < size_ - 1)
                {
                    MPI_Status temp_status;
                    int recv_buf = 0;
                    MPI_Recv(&recv_buf, 1, MPI_INT, MPI_ANY_SOURCE, INIT_TAG, world_, &temp_status);

                    int rank_check = temp_status.MPI_SOURCE;
                    if(available_procs_[rank_check - 1] == 1)
                        throw std::runtime_error("Error: received a ready message from " + std::to_string(rank_check) + " twice!\n");
                    else
                        available_procs_[rank_check - 1] = 1;

                    num_consecutive_procs_++;
                    count++;
                }

                //Check to make sure we got to everyone unneccessary due to logic above
            }
            else
            {
                //Send init message
                MPI_Send(&rank_, 1, MPI_INT, 0, INIT_TAG, world_);

            }

        }

        void schedule_job(std::string job_name, std::string function, std::map<std::string, chaiscript::Boxed_Value> groups, int num_procs)
        {
            std::map<std::string, int> new_map = convert_to_int_map(groups);
            //Scheduling logic goes here
            jobs_.emplace(job_name, function, new_map, num_procs);

            //job_id_[] = job_name;
        }

        bool control()
        {
            // Checking for complete jobs
            check_for_complete_jobs();

            check_for_intercommunication_requests();

            //while jobs and available processes, schedule jobs
            HensonJob next;
            if(!jobs_.empty())
                next = jobs_.front();

            while(!jobs_.empty() && next.num_procs <= num_consecutive_procs_)
            {
                //std::cout << "About to get next job from front of queue" << std::endl;

                //This tightly coupled loop is the reason I hate object orientated systems...looks nice right?
                run_next_job();
                //std::cout << "About to pop next job" << std::endl;
                jobs_.pop();
                if(!jobs_.empty())
                    next = jobs_.front();
            }

            return true;
        }



        void listen()
        {
            //TODO: Save ProcMap's current state

            MPI_Status job_status;

            //std::cout << "[" << rank_ << "]\t\t" << "About to probe!\n";
            int rank;
            MPI_Comm_rank(world_, &rank);
            //std::cout << "[" << rank_ << "]\t\t" << "Rank: " << rank << std::endl;
            MPI_Probe(0, MPI_ANY_TAG, world_, &job_status);
            //std::cout << "[" << rank_ << "]\t\t" << "Finished probe! " << std::endl;
            if(job_status.MPI_TAG == STOP_TAG)
            {
                //std::cout << "[" << rank_ << "]\t\t" << "Receiving Stop!" << std::endl;
                int temp;
                MPI_Recv(&temp, 1, MPI_INT, 0, STOP_TAG, world_, MPI_STATUS_IGNORE);
                is_active_ = false;
                //std::cout << "[" << rank_ << "]\t\t" << "Received Stop!" << std::endl;

            }
            else if(job_status.MPI_TAG == JOB_TAG)
            {

                int character_size;

                //std::cout << "[" << rank_ << "]\t\t" << "Received Job!\n";
                //Collect the function string
                MPI_Get_count(&job_status, MPI_CHAR, &character_size);
                std::vector<char> func_in(character_size + 1);

                MPI_Recv(func_in.data(), character_size, MPI_CHAR, 0, JOB_TAG, world_, MPI_STATUS_IGNORE);
                func_in[character_size] = '\0';

                /*std::cout << "[" << rank_ << "]\t\tfunc_in: ";
                for (auto elem : func_in) std::cout << elem;
                std::cout << std::endl;
                */


                //Collect the group string
                MPI_Probe(0, JOB_TAG, world_, &job_status);
                MPI_Get_count(&job_status, MPI_CHAR, &character_size);
                std::vector<char> group_in(character_size);
                MPI_Recv(group_in.data(), character_size, MPI_CHAR, 0, JOB_TAG, world_, MPI_STATUS_IGNORE);
                std::string raw_groups(group_in.begin(), group_in.end());

                /*std::cout << "[" << rank_ << "]\t\tgroup_in: ";
                for (auto elem : group_in) std::cout << elem;
                std::cout << std::endl;
                */


                int first_rank, last_rank;
                std::vector<int> ranks(2, 0);
                MPI_Recv(ranks.data(), 2, MPI_INT, 0, JOB_TAG, world_, MPI_STATUS_IGNORE);


                MPI_Probe(0, JOB_TAG, world_, &job_status);
                MPI_Get_count(&job_status, MPI_CHAR, &character_size);
                std::vector<char> job_name_in(character_size);
                MPI_Recv(job_name_in.data(), character_size, MPI_CHAR, 0, JOB_TAG, world_, MPI_STATUS_IGNORE);
                std::string job_name(job_name_in.begin(), job_name_in.end());

                //std::cout << "[" << rank_ << "]\t\tFirst received: " << ranks[0] << "\t\tLast received:  " << ranks[1] << std::endl;
                //Create job communicator
                fix_procmap(raw_groups, ranks, job_name);

                //std::cout << "[" << rank_ << "]\t\t" << "Procmap fixed\n";
                //Do job

                chaiscript::Boxed_Value returned_value = chai_->eval(func_in.data());
                if(!returned_value.get_type_info().is_undef())
                {
                    int job_size;
                    MPI_Comm_size(proc_map_->get_lowest_procmap()->world(), &job_size);

                    double buffer[2];
                    buffer[0] = static_cast<double>(job_size);
                    buffer[1] = chaiscript::boxed_cast<double>(returned_value);
                    //std::cout << "About to send back group finished message!" << std::endl;
                    MPI_Send(buffer, 2, MPI_DOUBLE, 0, JOB_FINISHED_TAG, world_);
                }


                proc_map_->clean_up_lowest_procmap();






                //std::cout << "Returned from chai eval!" << std::endl;

                /*
                int job_rank, job_size;
                MPI_Comm_size(proc_map_->world(), &job_size);
                MPI_Comm_rank(proc_map_->world(), &job_rank);
                int finished = 1;
                if(job_rank == 0)
                {
                    for(int k = 0; k < job_size - 1; ++k)
                    {
                        MPI_Recv(&finished, 1, MPI_INT, MPI_ANY_SOURCE, JOB_FINISHED_TAG, proc_map_->world(), MPI_STATUS_IGNORE);
                    }

                    MPI_Request temp;
                    double buffer[2];
                    buffer[0] = static_cast<double>(job_size);
                    buffer[1] = returned_value;
                    //std::cout << "About to send back group finished message!" << std::endl;
                    MPI_Isend(buffer, 2, MPI_DOUBLE, 0, JOB_FINISHED_TAG, world_, &temp);
                }
                else
                {
                    //std::cout << "Rank: " << local_rank << " sending finished message!" << std::endl;
                    MPI_Send(&finished, 1, MPI_INT, 0, JOB_FINISHED_TAG, proc_map_->world());
                }
                */
                //std::cout << "[" << rank_ << "]\t\t" << "Eval finished\n";

                //Send response back that job is complete
                //std::cout << "[" << rank_ << "]\t\t" << "Sent data!\n";
            }
            // TODO: Restore ProcMap to previous state
        }


        void check_for_complete_jobs()
        {
            MPI_Status job_status;
            int check_flag = 1;

            while(check_flag)
            {
                //std::cout << "About to Probe for finished jobs!" << std::endl;
                MPI_Iprobe(MPI_ANY_SOURCE, JOB_FINISHED_TAG, world_, &check_flag, &job_status);
                if(check_flag)
                {
                    //The first is the size, the second the answer
                    double recv_var[2];
                    MPI_Recv(recv_var, 2, MPI_DOUBLE, job_status.MPI_SOURCE, JOB_FINISHED_TAG, world_, MPI_STATUS_IGNORE);
                    double end_time = MPI_Wtime();

                    auto elem = job_id_.begin();
                    for(; elem != job_id_.end(); elem++)
                    {
                        int rank_returned = job_status.MPI_SOURCE;
                        if(rank_returned >= elem->second.first && rank_returned <= elem->second.second)
                        {
                            //Sanity check to make sure everything is okay, left over from deprecated code
                            if(static_cast<int>(recv_var[0]) != elem->second.second - elem->second.first + 1)
                            {
                                throw std::runtime_error("Error: size returned doesn't match job bookeeping! Size returned: " + std::to_string(static_cast<int>(recv_var[0])) + " Size: "
                                + std::to_string(elem->second.second - elem->second.first + 1));
                            }



                            for(int i = elem->second.first; i <= elem->second.second; ++i)
                                available_procs_[i - 1] = 1;

                            //std::cout << "Received an answer from rank: " << job_status.MPI_SOURCE << std::endl;

                            job_times_[elem->first].second = end_time;
                            stack_.emplace_back(elem->first, recv_var[1]);
                            break;
                        }
                    }

                    if(elem == job_id_.end())
                    {
                        throw std::runtime_error("Error: could not find rank in scheduler bookeeping");
                    }
                    job_id_.erase(elem);
                    //stack_.emplace_back(job_id_.at(job_status.MPI_SOURCE), recv_var[1]);


                    num_consecutive_procs_ = calculate_num_consecutive_procs();

                }
            }
        }


        void finish()
        {
            while(has_unfinished_jobs())
            {
                //std::cout << "unfinished jobs exist!" << std::endl;
                check_for_complete_jobs();
            }

            //std::cout << "Found outstanding jobs, sending stop signal!" << std::endl;

            print_job_times();
            send_stop_sched_signal();

            //std::cout << "Sent stop signal to everyone!" << std::endl;
        }


        int  get_size()                 { return size_; }
        int  get_schedule_rank()        { return rank_; }
        bool is_active()                { return is_active_; }
        bool is_job_queue_empty()       { return jobs_.empty(); }
        bool is_controller()            { return (rank_ == 0 ? true : false); }
        bool is_stack_empty()           { return stack_.empty(); }
        StackElement next_on_stack()    { return stack_.back(); }
        void pop_stack()                { stack_.pop_back(); }

    private:

        void print_job_times()
        {
            for(auto it = job_times_.begin(); it != job_times_.end(); ++it)
                std::cout << it->first << " job_time: " << (it->second.second - it->second.first) << std::endl;
        }

        void check_for_intercommunication_requests()
        {

            MPI_Status job_status;
            int check_flag = 1;

            while(check_flag)
            {
                MPI_Iprobe(MPI_ANY_SOURCE, INTERCOMM_FIND_TAG, world_, &check_flag, &job_status);
                if(check_flag)
                {
                    //The first is the size, the second the answer
                    int size_of_recv = -1;
                    MPI_Get_count(&job_status, MPI_CHAR, &size_of_recv);

                    std::vector<char> buffer(size_of_recv);
                    MPI_Recv(buffer.data(), size_of_recv, MPI_CHAR, job_status.MPI_SOURCE, INTERCOMM_FIND_TAG, world_, MPI_STATUS_IGNORE);

                    try
                    {
                        std::string temp_string(buffer.begin(), buffer.end());
                        auto job_processor_pair = job_id_.at(temp_string);
                        int scheduler_rank = job_processor_pair.first;

                        MPI_Group scheduler_group, world_group;
                        MPI_Comm_group(world_, &scheduler_group);
                        MPI_Comm_group(MPI_COMM_WORLD, &world_group);

                        int world_rank = -1;
                        MPI_Group_translate_ranks(scheduler_group, 1, &scheduler_rank, world_group, &world_rank);

                        //Let the other side know that something went wrong
                        if(world_rank == MPI_UNDEFINED)
                            throw std::runtime_error("Error: could not translate rank: " + std::to_string(scheduler_rank));

                        MPI_Send(&world_rank, 1, MPI_INT, job_status.MPI_SOURCE, INTERCOMM_FIND_TAG, world_);

                    }
                    catch(std::out_of_range e)
                    {
                        int nothing = -1;
                        MPI_Send(&nothing, 1, MPI_INT, job_status.MPI_SOURCE, INTERCOMM_FIND_TAG, world_);

                    }
                }
            }



        }

        void run_next_job()
        {
            HensonJob next = jobs_.front();
            //Search for free procs
            int last = -1;
            int first = -1;
            //check_for_complete_jobs();
            int procs_in_a_row = 0;
            for(int i = 0; i < available_procs_.size(); ++i)
            {
                if(available_procs_[i] == 1)
                {
                    if(first == -1)
                        first = i + 1;
                    procs_in_a_row++;
                }
                else
                {
                    first = -1;
                    procs_in_a_row = 0;
                }

                if(procs_in_a_row == next.num_procs)
                {
                    last = i + 1;
                    break;
                }

            }
            if(last == -1)
                throw std::runtime_error("Error: No correct number of procs in a row found, error in logic\n");



            //Converting map into string, don't judge me, everything is already written for a
            //string and this keeps me from transmitting a map piecmeal from one side of the
            //MPI comm to the other
            std::string group_string = "";
            for(auto it = next.groups.begin(); it != next.groups.end(); it++)
            {
                std::string temp = (it->second <= 0 ? "" : "=" + std::to_string(it->second));

                if(std::next(it, 1) == next.groups.end())
                    group_string = group_string + it->first + temp;
                else
                    group_string = group_string + it->first + temp + " ";
            }


            // Send job to free procs
            log_->debug("Sending job to procs {} through {}", first, last);
            std::vector<int> first_last_procs(2, 0);
            first_last_procs[0] = first;
            first_last_procs[1] = last;

            //Set up the timer and get the first reading
            double start_time;
            start_time = MPI_Wtime();
            for(int i = first; i <= last; ++i)
            {
                MPI_Send(next.function.c_str(), next.function.size(), MPI_CHAR, i, JOB_TAG, world_);
                MPI_Send(group_string.c_str(), group_string.size(), MPI_CHAR, i, JOB_TAG, world_);
                MPI_Send(first_last_procs.data(), 2, MPI_INT, i, JOB_TAG, world_);
                MPI_Send(next.job_name.c_str(), next.job_name.size(), MPI_CHAR, i, JOB_TAG, world_);
                available_procs_[i - 1] = 0;
            }
            num_consecutive_procs_ = calculate_num_consecutive_procs();

            //Erasing remnants of the last job if there are any
            job_id_.erase(next.job_name);
            job_id_[next.job_name] = std::make_pair(first, last);
            job_times_[next.job_name] = std::make_pair(start_time, 0.0);
        }

        //Used to keep track of number of consecutive procs
        int calculate_num_consecutive_procs()
        {
            int max_consecutive = 0;
            int curr_consecutive = 0;
            for(int i = 0; i < available_procs_.size(); ++i)
            {
                if(available_procs_[i] == 1)
                    curr_consecutive++;
                else
                    curr_consecutive = 0;


                if(curr_consecutive > max_consecutive)
                    max_consecutive = curr_consecutive;
            }

            num_consecutive_procs_ = max_consecutive;
            return num_consecutive_procs_;
        }



        bool has_unfinished_jobs()
        {
            for(int i = 0; i < available_procs_.size(); ++i)
            {
                if(available_procs_[i] == 0)
                {
                    //std::cout << "Unavailable: " << i << std::endl;
                    return true;
                }
            }
            //std::cout << "All outstanding_jobs received!\n";

            return false;
        }

        void send_stop_sched_signal()
        {
            for(int i = 0; i < available_procs_.size(); i++)
            {
                MPI_Request req;
                int finished = 1;
                MPI_Send(&finished, 1, MPI_INT, (i + 1), STOP_TAG, world_);
            }
        }

        std::map<std::string, int> convert_to_int_map(const std::map<std::string, chaiscript::Boxed_Value>& groups)
        {
            std::map<std::string, int> new_map;
            std::transform(groups.begin(), groups.end(), std::inserter(new_map, new_map.begin()), [](const std::pair<std::string, chaiscript::Boxed_Value>& old_pair)
                                                                                       { int temp = chaiscript::Boxed_Number(old_pair.second).get_as<int>();
                                                                                         std::pair<std::string, int> new_pair(old_pair.first, temp);
                                                                                         return new_pair; });
            if(!groups.empty() && new_map.empty())
               throw std::runtime_error("Error: new_map was not transformed correctly\n");

            return new_map;
        }


        int find_remote_rank(const std::string& execution_group_name)
        {

            std::vector<char> data_buffer(execution_group_name.begin(), execution_group_name.end());
            MPI_Send(data_buffer.data(), data_buffer.size(), MPI_CHAR, 0, INTERCOMM_FIND_TAG, world_);

            int remote_rank = -1;
            MPI_Recv(&remote_rank, 1, MPI_INT, 0, INTERCOMM_FIND_TAG, world_, MPI_STATUS_IGNORE);

            if(remote_rank == -1)
                throw std::runtime_error("Error: remote group was not found for some reason!  Looking for: " + execution_group_name);

            return remote_rank;
        }


        void fix_procmap(std::string raw_groups, const std::vector<int>& first_last_procs, std::string& jb_name)
        {

            //Parse args to get them into right format
            std::vector<std::string> group_parsed;

            int prev = -1;
            size_t pos = 0;
            while (pos != std::string::npos)
            {

                pos = raw_groups.find(' ', pos + 1);

                if(!std::isspace(raw_groups[prev + 1]))
                    group_parsed.emplace_back(raw_groups.begin() + prev + 1, pos == std::string::npos ? raw_groups.end() : raw_groups.begin() + pos);

                prev = pos;
            }

            //std::cout << "[" << rank_ << "]\t\tParsed raw groups, resetting procmap\n";
            //std::cout << "[" << rank_ << "]\t\tFirst proc: " << first_last_procs[0] << "\t\tLast proc: " << first_last_procs[1] << std::endl;

            MPI_Group job_group;
            create_group(first_last_procs[0], first_last_procs[1], world_, &job_group);

            MPI_Comm newcomm = MPI_COMM_NULL;

//I think this if check is pointless, but I'm not sure
            int low = first_last_procs[0];
            int high = first_last_procs[1];
            if(rank_ <= high && rank_ >= low)
            {
                non_collective_comm_create(job_group, world_, NONCOLLECTIVE_TAG, &newcomm);
            }
            else
                throw std::runtime_error("Error: rank assigned job but not part of job intracreate");

            //std::cout << "[" << rank_ << "]\t\tCreated job_comm!" << std::endl;


            if(newcomm == MPI_COMM_NULL)
            {
                std::string temp = std::to_string(first_last_procs[0]) + ":\tJob communicator did not get created error!!!";
                throw std::runtime_error(temp);
            }

            proc_map_->create_child_procmap(group_parsed, first_last_procs, newcomm);
            proc_map_->get_lowest_procmap()->set_job_name(jb_name);
            //proc_map_->set_intercomm_callback(std::bind(&henson::Scheduler::find_remote_rank, this, std::placeholders::_1));
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
        int                                                 num_consecutive_procs_;
        const int                                           MAX_JOBS_;

        std::queue<HensonJob>                               jobs_;
        //Don't forget the available_procs_ array is MPI_SIZE - 1
        std::vector<int>                                    available_procs_;

        std::map<std::string, std::pair<int,int>>           job_id_;
        std::map<std::string, std::pair<double, double>>    job_times_;
        std::vector<StackElement>                           stack_;

        chaiscript::ChaiScript*                             chai_;
        ProcMap*                                            proc_map_;

        bool                                                is_active_;

        std::shared_ptr<spd::logger>                        log_ = spd::get("henson");
};

}

#endif
