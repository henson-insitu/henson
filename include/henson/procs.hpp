#ifndef HENSON_PROCS_HPP
#define HENSON_PROCS_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>

#include <mpi.h>

#include <fmt/format.h>

namespace henson
{

// Create this helper, so that MPI environment can be managed using RAII.  This
// way when ProcMap goes out of scope in main, its destructor won't be called
// after MPI_Finalize.
struct MPIEnvironment
{
        MPIEnvironment(int* argc, char*** argv)         { MPI_Init(argc, argv); }
        ~MPIEnvironment()                               { MPI_Finalize(); }
};

class ProcMap
{
    public:
        struct Group
        {
            int leader;     // stores the ranks of the "root" processes in the subcommunicators (used for creating intercomms)
            int size;
        };
        using GroupMap = std::map<std::string, Group>;

        struct InterIntraComm
        {
            MPI_Comm inter_comm;
            MPI_Comm intra_comm;
        };
        using IntercommCache = std::map<std::string, InterIntraComm>;

        struct Level
        {
            Level(MPI_Comm world_, MPI_Comm local_, GroupMap groups_, int color_, std::string group_, IntercommCache cache_):
                world(world_), local(local_), groups(groups_), color(color_), group(group_), intercomm_cache(cache_)   {}

            MPI_Comm            world, local;
            GroupMap            groups;
            int                 color;
            std::string         group;
            IntercommCache      intercomm_cache;
        };
        using Levels = std::vector<Level>;

        using Vector = std::vector< std::pair<std::string, int> >;

    public:
                    ProcMap(MPI_Comm world, const Vector& procs)
        {
            if (world == MPI_COMM_WORLD)
                throw std::runtime_error("Cannot use MPI_COMM_WORLD in procmap (it will break intercommunicator creation); need to make a copy before passing it here");

            extend(world, procs);
        }
                    ~ProcMap()                                              { pop_back(); }

        // better safe than sorry
                    ProcMap(const ProcMap&)                                 =delete;
        ProcMap&    operator=(const ProcMap&)                               =delete;
                    ProcMap(const ProcMap&&)                                =delete;
        ProcMap&    operator=(const ProcMap&&)                              =delete;

        using Lvl = Level;  // for formatting
        const Lvl&  level() const                                           { return levels_.back(); }
        Lvl&        level()                                                 { return levels_.back(); }
        MPI_Comm    world() const                                           { return level().world; }
        MPI_Comm    local() const                                           { return level().local; }
        int         world_rank() const                                      { int rank; MPI_Comm_rank(world(), &rank); return rank; }
        int         local_rank() const                                      { int rank; MPI_Comm_rank(local(), &rank); return rank; }
        int         color() const                                           { return level().color; }
        std::string group() const                                           { return level().group; }
        int         leader(const std::string& name) const                   { return level().groups.find(name)->second.leader; }
        int         size(const std::string& name) const                     { return level().groups.find(name)->second.size; }
        bool        is_leader(int rank) const                               { for (GroupMap::const_iterator it = level().groups.begin(); it != level().groups.end(); ++it) if (it->second.leader == rank) return true; return false; }

        MPI_Comm    intercomm(const std::string& to, int tag = 0)
        {
            // It's necessary to cache intercomms: a puppet may call this
            // function multiple times (e.g., if it's restarted from scratch),
            // but subsequent calls to MPI_Intercomm_create cannot re-use the
            // same tag.

            // Search for the intercomm from the bottom up
            for (auto lit = levels_.rbegin(); lit != levels_.rend(); ++lit)
            {
                IntercommCache::const_iterator it = lit->intercomm_cache.find(to);
                if (it != lit->intercomm_cache.end())
                    return it->second.inter_comm;
            }

            // If not found, create the intercomm at the lowest level
            MPI_Comm inter_comm;
            MPI_Intercomm_create(local(), 0, world(), leader(to), tag, &inter_comm);

            MPI_Comm intra_comm;
            int remote_leader = leader(to);
            int local_leader  = leader(group());
            MPI_Intercomm_merge(inter_comm, local_leader < remote_leader, &intra_comm);

            level().intercomm_cache[to] = { inter_comm, intra_comm };

            return inter_comm;
        }

        MPI_Comm    intracomm(const std::string& to)
        {
            // Search for the intracomm from the bottom up
            for (auto lit = levels_.rbegin(); lit != levels_.rend(); ++lit)
            {
                IntercommCache::const_iterator it = lit->intercomm_cache.find(to);
                if (it != lit->intercomm_cache.end())
                    return it->second.intra_comm;
            }
            throw std::runtime_error("No intracomm associated with " + to);
        }

        void        extend(MPI_Comm world, const Vector& procs);        //!< add a level at the bottom
        void        pop_back();                                         //!< remove a level at the bottom

        static Vector   parse_procs(const std::vector<std::string>& procs_sizes, int size, std::vector<std::string> all_groups = {});

    private:
        Levels      levels_;
};


inline
void
ProcMap::extend(MPI_Comm world, const Vector& procs)
{
    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    GroupMap groups;
    int color = 0;
    int total_procs = 0;
    std::string group;
    for (size_t i = 0; i < procs.size(); ++i)
    {
        const std::string&  name = procs[i].first;
        int                 n    = procs[i].second;

        groups[name]  = { total_procs, n };

        if (rank >= total_procs)
            group = name;      // repeatedly overwritten

        total_procs += n;
        if (rank >= total_procs)
            ++color;

    }
    if (total_procs != size)
        throw std::runtime_error(fmt::format("Requested number of processors doesn't match MPI size: {} vs {}", total_procs, size));

    MPI_Comm local;
    MPI_Comm_split(world, color, 0, &local);

    levels_.emplace_back(world, local, groups, color, group, IntercommCache());
}

inline
void
ProcMap::pop_back()
{
    // destroy communicators
    for (auto& x : level().intercomm_cache)
    {
        MPI_Comm_free(&x.second.intra_comm);
        MPI_Comm_free(&x.second.inter_comm);
    }
    MPI_Comm_free(&level().local);
    // NB: we did not create level().world, so we won't be destroying it either

    levels_.pop_back();
}

inline
ProcMap::Vector
ProcMap::parse_procs(const std::vector<std::string>& procs_sizes, int size, std::vector<std::string> all_groups)
{
    ProcMap::Vector     procs;
    int                 total_procs = 0;
    std::set<std::string> unspecified_size;
    for (std::string procs_size : procs_sizes)
    {
        size_t eq_pos = procs_size.find('=');
        if (eq_pos == std::string::npos)
        {
            unspecified_size.insert(procs_size);
            continue;
        }
        int sz = std::stoi(procs_size.substr(eq_pos + 1, procs_size.size() - eq_pos));
        procs.emplace_back(procs_size.substr(0, eq_pos), sz);
        total_procs += sz;
    }

    if (total_procs > size)
        throw std::runtime_error("Specified procs exceed MPI size");

    // record assigned groups
    std::set<std::string>   assigned_procs;
    for (auto& x : procs)
        assigned_procs.insert(x.first);

    for (auto& x : all_groups)
        if (assigned_procs.find(x) == assigned_procs.end())
            unspecified_size.insert(x);

    // assign unassigned procs
    int unassigned = unspecified_size.size();
    for (auto& x : unspecified_size)
        procs.emplace_back(x, (size - total_procs) / unassigned);

    return procs;
}

}

#endif
