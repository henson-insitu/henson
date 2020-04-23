#include <henson/context.h>
#include <henson/context.hpp>
#include <henson/procs.hpp>

#ifdef USE_BOOST
#include <boost/context/execution_context.hpp>
namespace bc = boost::context;
typedef     bc::execution_context<void*>    context_t;
#else
#include <henson/coro.h>
typedef     coro_context                    context_t;
#endif


static context_t* parent = 0;
static context_t* local  = 0;

static henson::ProcMap* procmap = 0;

static int*             stop = 0;

__attribute__ ((visibility ("hidden")))
int  henson_active()
{
    return procmap != 0;
}

__attribute__ ((visibility ("hidden")))
void henson_yield()
{
    if (parent == 0 || local == 0)      // not running under henson; do nothing
        return;

#ifdef USE_BOOST
    *parent = std::move(std::get<0>((*parent)(0)));
#else
    coro_transfer(local, parent);
#endif
}

void henson_set_contexts(void* p, void* l)
{
    parent = (context_t*) p;
    local  = (context_t*) l;
}

void henson_set_procmap(void* pm)
{
    procmap = static_cast<henson::ProcMap*>(pm);
}

__attribute__ ((visibility ("hidden")))
henson::ProcMap* henson::get_procmap()
{
    return procmap;
}

__attribute__ ((visibility ("hidden")))
MPI_Comm henson_get_world()
{
    if (!procmap)
        return MPI_COMM_WORLD;
    else
        return procmap->local();
}

__attribute__ ((visibility ("hidden")))
MPI_Comm    henson_get_intercomm(const char* to)
{
    return procmap->intercomm(to);
}

__attribute__ ((visibility ("hidden")))
MPI_Comm    henson_get_intracomm(const char* to)
{
    return procmap->intracomm(to);
}

void        henson_set_stop(int* s)
{
    stop = s;
}

__attribute__ ((visibility ("hidden")))
int         henson_stop()
{
    if (!stop) return 0;
    return *stop;
}
