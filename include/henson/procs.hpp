#ifndef HENSON_PROCS_HPP
#define HENSON_PROCS_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <functional>

#include <henson/mpi-noncollective-comm.h>

namespace henson
{

struct CommList
{
    CommList():inter_comm(MPI_COMM_NULL), intra_comm(MPI_COMM_NULL), translated_rank(-1) {}

    CommList(MPI_Comm inter, MPI_Comm intra, int rank): inter_comm(inter),
                                                        intra_comm(intra),
                                                        translated_rank(rank){}

    MPI_Comm inter_comm;
    MPI_Comm intra_comm;
    int      translated_rank;
};

class ProcMap
{
    public:
        typedef     std::vector< std::pair<std::string, int> >              Vector;
        typedef     std::map<std::string, int>                              Map;
        typedef     std::map<std::string, std::pair<int, int> >             ExtendedMap;
        typedef     std::map<std::string, CommList>                         IntercommCache;
        typedef     std::map<std::string, MPI_Comm>                         SubIntercommCache;


    public:
        ProcMap(MPI_Comm world, const std::vector<std::string>& procs_sizes, int size):
            color_(0), disable_local_(false), parent_(NULL), child_(NULL)
        {
            MPI_Comm_rank(world, &job_rank_);

            auto procs = parse_procs(procs_sizes, size);
            this->fill(world, procs);
        }


                    ProcMap(MPI_Comm world, const Vector& procs):
                        color_(0), disable_local_(false), parent_(NULL), child_(NULL)
        {

            MPI_Comm_rank(world, &job_rank_);

            this->fill(world, procs);
        }


        MPI_Comm            world() const                                                               { return world_; }
        MPI_Comm            local() const                                                               { if (!disable_local_) return local_; else return world_; }
        int                 color() const                                                               { return color_; }
        int                 leader(const std::string& name) const                                       { return procs_.find(name)->second.first; }
        int                 size(const std::string& name) const                                         { return (procs_.find(name)->second.second - procs_.find(name)->second.first + 1); }
        int                 get_local_rank() const                                                      { int rank; MPI_Comm_rank(local_, &rank); return rank; }
        int                 get_job_rank() const                                                        { return job_rank_; }
        henson::ProcMap *   get_child()                                                                 { return child_; }
        henson::ProcMap *   get_parent()                                                                { return parent_; }
        void                set_child(henson::ProcMap* child)                                           { child_ = child; }
        void                set_parent(henson::ProcMap* parent)                                         { parent_ = parent; }
        IntercommCache&     get_intercomm_cache()                                                       { return intercomm_cache_; }

        bool        isInGroup(const std::string& group_name) const                              { return (job_rank_ >= procs_.find(group_name)->second.first &&
                                                                                                          job_rank_ <= procs_.find(group_name)->second.second); }
        std::string &       get_job_name()                                                              { return job_name_; }
        void                set_job_name(std::string jb_name)                                           { job_name_ = jb_name; }

        henson::ProcMap * get_lowest_procmap()
        {
            henson::ProcMap * lowest_procmap = this;
            while(lowest_procmap->get_child()) lowest_procmap = lowest_procmap->get_child();
            return lowest_procmap;
        }

        void clean_up_lowest_procmap()
        {
            if(this->get_child() != this->get_lowest_procmap())
                throw std::runtime_error("Error: ProcMaps are nested more than two deep!  Not handled!");

            MPI_Comm temp = this->get_lowest_procmap()->local();
            MPI_Comm_free(&temp);

            temp = this->get_lowest_procmap()->world();
            MPI_Comm_free(&temp);

            delete child_;
            child_ = NULL;
        }


        int get_local_size() const
        {
            for(auto elem : procs_)
            {
                if(job_rank_ >= elem.second.first && job_rank_ <= elem.second.second)
                    return this->size(elem.first);
            }
            return 0;
        }
        //This logic was messed up I think in Henson as well
        bool       is_leader(int rank) const
        {
            bool isLeader = false;
            for (ExtendedMap::const_iterator it = procs_.begin(); it != procs_.end(); ++it)
            {
                if (it->second.first == rank)
                {
                    isLeader = true;
                    break;
                }

            }
                return isLeader;
        }


        MPI_Comm    search_for_cached_intercomm(henson::ProcMap* pm, const std::string& to, int tag)
        {
            IntercommCache::const_iterator it = pm->get_intercomm_cache().find(to);
            if (it != pm->get_intercomm_cache().end())
            {
                //Job seeking to talk to top level
                if(pm->get_parent() == NULL && pm->get_child() != NULL)
                {
                    //Sanity check
                    if(it->second.intra_comm == MPI_COMM_NULL)
                        throw std::runtime_error("Error: no intracommunicator when one should have been set up already!");

                    MPI_Comm new_comm;
                    int local_rank = -1;
                    MPI_Comm_rank(local_, &local_rank);
                    if(local_rank == 0)
                    {
                        int intra_rank = -1;
                        MPI_Comm_rank(it->second.intra_comm, &intra_rank);
                        MPI_Send(&intra_rank, 1, MPI_INT, it->second.translated_rank, tag, it->second.intra_comm);
                    }

                    MPI_Intercomm_create(local_, 0, it->second.intra_comm, it->second.translated_rank, tag, &new_comm);

                    intercomm_cache_[to] = CommList(new_comm, MPI_COMM_NULL, -1);

                    return new_comm;
                }
                else
                {
                    return it->second.inter_comm;
                }
            }
            else
            {
                if(pm->get_parent() == NULL)
                    return MPI_COMM_NULL;
                else
                    return search_for_cached_intercomm(pm->get_parent(), to, tag);
            }

        }





        MPI_Comm    intercomm(const std::string& to, int tag = 0)
        {
            // It's necessary to cache intercomms: a puppet may call this
            // function multiple times (e.g., if it's restarted from scratch),
            // but subsequent calls to MPI_Intercomm_create hang for some
            // reason.
            MPI_Comm cached_comm;
            int the_local_rank = -1;
            MPI_Comm_rank(local_, &the_local_rank);
            cached_comm = search_for_cached_intercomm(this, to, tag);


            if(cached_comm != MPI_COMM_NULL)
                return cached_comm;


            MPI_Comm comm;


            ExtendedMap::const_iterator it = procs_.find(to);
            if(it != procs_.end())
            {
                disable_local_ = true;
                MPI_Group combined_group, local_group, remote_group, parent_group;
                MPI_Comm newcomm = MPI_COMM_NULL;

                //Creating the intercomm between siblings
                MPI_Intercomm_create(local_, 0, world(), procs_.find(to)->second.first, tag, &comm);

                MPI_Comm_remote_group(comm, &remote_group);
                MPI_Comm_group(world_, &parent_group);
                MPI_Comm_group(local_, &local_group);
                //I'm not convinced this works allways, but it doesn't throw up an error so it should be fine

                int local_size = -1;
                MPI_Comm_size(local_, &local_size);
                std::vector<int> group_array;

                //We are going to assume that we did the bookeeping correctly
                int local_rank = 0;
                int translated_local_rank = -1;
                MPI_Group_translate_ranks(local_group, 1, &local_rank, parent_group, &translated_local_rank);

                //establish a group array in order using what we know
                if(procs_.at(to).first < translated_local_rank)
                {
                    for(int i = procs_.at(to).first; i <= procs_.at(to).second; ++i)
                        group_array.push_back(i);

                    for(int i = translated_local_rank; i < translated_local_rank + local_size; ++i)
                        group_array.push_back(i);
                }
                else
                {
                    for(int i = translated_local_rank; i < translated_local_rank + local_size; ++i)
                        group_array.push_back(i);

                    for(int i = procs_.at(to).first; i <= procs_.at(to).second; ++i)
                        group_array.push_back(i);
                }

                //MPI_Group_translate_ranks(local_group, 1, &local_rank, combined_group, &combined_group_rank);
                MPI_Group_incl(parent_group, group_array.size(), group_array.data(), &combined_group);

                //Creating the intracomm between adults
                non_collective_comm_create(combined_group, world_, 77, &newcomm);
                int remote_rank = 0;
                int translated_remote_rank = -1;
                MPI_Group_translate_ranks(remote_group, 1, &remote_rank, combined_group, &translated_remote_rank);
                disable_local_ = false;

                intercomm_cache_[to] = CommList(comm, newcomm, translated_remote_rank);
                return comm;
            }
            else
            {

                throw std::runtime_error("Error: intercomm name: " + to + " couldn't be found locally or cached upwards!");
            }
        }


        MPI_Comm    intracomm(const std::string& to)
        {
            IntercommCache::const_iterator it = get_intercomm_cache().find(to);
            if(it != get_intercomm_cache().end())
                return it->second.intra_comm;
            else
                throw std::runtime_error("Error: no intracomm associated with " + to);
        }


        //If this is ever used for more than one job at a time, the tag will need to be different for each use
        MPI_Comm    subintercomm(const char* to, MPI_Comm intra_comm, int remote_leader, int tag = 0)
        {
            SubIntercommCache::const_iterator it = sub_intercomm_cache_.find(to);
            if(it != sub_intercomm_cache_.end())
                return it->second;

            MPI_Comm newcomm = MPI_COMM_NULL;
            MPI_Intercomm_create(local_, 0, intra_comm, remote_leader, tag, &newcomm);

            sub_intercomm_cache_[to] = newcomm;
            return newcomm;
        }

        void       destroy_subintercomm(const char* to)
        {
            SubIntercommCache::iterator it = sub_intercomm_cache_.find(to);
            if(it != sub_intercomm_cache_.end())
            {
                MPI_Comm_free(&(it->second));
                sub_intercomm_cache_.erase(it);
            }
            else
            {
                throw std::runtime_error("Error: tried to destroy subintercomm " + std::string(to) + " but there was no such thing.");
            }
        }



        void fill(MPI_Comm world, const Vector& procs)
        {
            world_ = world;
            int size;
            MPI_Comm_rank(world_, &job_rank_);
            MPI_Comm_size(world_, &size);

            int total_procs = 0;
            for (size_t i = 0; i < procs.size(); ++i)
            {
                const std::string&  name = procs[i].first;
                int                 n    = procs[i].second;

                procs_[name].first   = total_procs;
                procs_[name].second  = n + total_procs - 1;
                //leaders_[name]       = total_procs;

                total_procs += n;
                if (job_rank_ >= total_procs)
                    ++color_;

            }
            if (total_procs != size)
                throw std::runtime_error("Requested number of processors doesn't match MPI size");

            //TODO:reenable thhis
            MPI_Comm_split(world_, color_, 0, &local_);
            //local_ = world;

        }


        void create_child_procmap(const std::vector<std::string>& procs_sizes, const std::vector<int>& first_last_procs, MPI_Comm job_world)
        {
            //This is done this way so the MPI can make a single call
            int size = first_last_procs[1] - first_last_procs[0] + 1;
            henson::ProcMap * new_child = new ProcMap(job_world, procs_sizes, size);
            set_child(new_child);
            new_child->set_parent(this);

        }



    private:

        Vector parse_procs(const std::vector<std::string>& procs_sizes, int size)
        {
             Vector procs;
             std::set<std::string> unassigned_proc;
             int total_assigned_procs = 0;
             for (std::string proc_size : procs_sizes)
             {
                 size_t eq_pos = proc_size.find('=');
                 if (eq_pos == std::string::npos)
                 {
                     unassigned_proc.insert(proc_size);
                 }
                 else
                 {
                    int sz = std::stoi(proc_size.substr(eq_pos + 1, proc_size.size() - eq_pos));
                    procs.emplace_back(proc_size.substr(0, eq_pos), sz);
                    total_assigned_procs += sz;
                 }
             }


            int number_unassigned = unassigned_proc.size();
            int count = 0;
            int assigned_here = 0;
            while(!unassigned_proc.empty())
            {
                int nprocs = 0;
                if((((size - total_assigned_procs) % number_unassigned) - count) != 0)
                {
                    nprocs = ((size - total_assigned_procs) / number_unassigned) + 1;
                    ++count;
                }
                else
                    nprocs = ((size - total_assigned_procs) / number_unassigned);


                auto it = unassigned_proc.begin();
                procs.emplace_back(*it, nprocs);
                unassigned_proc.erase(it);
                assigned_here += nprocs;

            }

            if(size != total_assigned_procs + assigned_here)
                throw std::runtime_error("Abort: some of the processors are unassigned!");

            return procs;


        }


        MPI_Comm                                                                world_, local_, dup_world_;
        int                                                                     job_rank_;
        ExtendedMap                                                             procs_;           //The leader is now stored as the first element of the pair in procs_
        //Map                                                                     leaders_;       // stores the ranks of the "root" processes in the subcommunicators (used for creating intercomms)
        int                                                                     color_;
        //std::function<int(const std::string&)>                                  intercomm_callback_func_;
        henson::ProcMap *                                                       parent_;
        henson::ProcMap *                                                       child_;
        SubIntercommCache                                                       sub_intercomm_cache_;
        std::string                                                             job_name_;

        // NB: this makes ProcMap not thread safe
        mutable bool            disable_local_;
        mutable IntercommCache  intercomm_cache_;


};

}

#endif
