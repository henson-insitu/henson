#ifndef HENSON_PROCS_HPP
#define HENSON_PROCS_HPP

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace henson
{

class ProcMap
{
    public:
        typedef     std::vector< std::pair<std::string, int> >      Vector;
        typedef     std::map<std::string, int>                      Map;
        typedef     std::map<std::string, MPI_Comm>                 IntercommCache;

    public:
                    ProcMap(MPI_Comm world, const Vector& procs):
                        world_(world), color_(0), disable_local_(false)
        {
            int rank, size;
            MPI_Comm_rank(world, &rank);
            MPI_Comm_size(world, &size);

            int total_procs = 0;
            for (size_t i = 0; i < procs.size(); ++i)
            {
                const std::string&  name = procs[i].first;
                int                 n    = procs[i].second;

                procs_[name]   = n;
                leaders_[name] = total_procs;

                total_procs += n;
                if (rank >= total_procs)
                    ++color_;

            }
            if (total_procs != size)
                throw std::runtime_error("Requested number of processors doesn't match MPI size");

            MPI_Comm_split(world_, color_, 0, &local_);
        }

        MPI_Comm    world() const                                           { return world_; }
        MPI_Comm    local() const                                           { if (!disable_local_) return local_; else return world_; }
        int         color() const                                           { return color_; }
        int         leader(const std::string& name) const                   { return leaders_.find(name)->second; }
        int         size(const std::string& name) const                     { return procs_.find(name)->second; }
        bool        is_leader(int rank) const                               { for (Map::const_iterator it = leaders_.begin(); it != leaders_.end(); ++it) if (it->second == rank) return true; return false; }

        MPI_Comm    intercomm(const std::string& to, int tag = 0) const
        {
            // It's necessary to cache intercomms: a puppet may call this
            // function multiple times (e.g., if it's restarted from scratch),
            // but subsequent calls to MPI_Intercomm_create hang for some
            // reason.
            IntercommCache::const_iterator it = intercomm_cache_.find(to);
            if (it != intercomm_cache_.end())
                return it->second;

            MPI_Comm comm;
            // a hack to avoid using PMPI_Intercomm_create directly;
            // inside MPI_Init_thread, local() gets called to get the "world" communicator;
            // in this case, we need to provide the honest world_
            disable_local_ = true;
            MPI_Intercomm_create(local_, 0, world(), leaders_.find(to)->second, tag, &comm);
            disable_local_ = false;
            intercomm_cache_[to] = comm;
            return comm;
        }

    private:
        MPI_Comm            world_, local_;
        Map                 procs_;
        Map                 leaders_;       // stores the ranks of the "root" processes in the subcommunicators (used for creating intercomms)
        int                 color_;

        // NB: this makes ProcMap not thread safe
        mutable bool            disable_local_;
        mutable IntercommCache  intercomm_cache_;
};

}

#endif
