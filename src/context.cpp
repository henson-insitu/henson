#include <henson/context.h>
#include <henson/procs.hpp>

#include <coro.h>

typedef     coro_context      context_t;

static context_t* parent = 0;
static context_t* local  = 0;

static henson::ProcMap* procmap = 0;

static int*             stop = 0;

int  henson_active()
{
    return (parent != 0 && local != 0);
}

void henson_yield()
{
    if (parent == 0 || local == 0)      // not running under henson; do nothing
        return;
    coro_transfer(local, parent);
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

MPI_Comm henson_get_world()
{
    if (!procmap)
        return MPI_COMM_WORLD;
    else
        return procmap->local();
}

MPI_Comm    henson_get_intercomm(const char* to)
{
    return procmap->intercomm(to);
}

void        henson_set_stop(int* s)
{
    stop = s;
}

int         henson_stop()
{
    if (!stop) return 0;
    return *stop;
}
