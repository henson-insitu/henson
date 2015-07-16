#include <henson/context.h>
#include <henson/procs.hpp>

#include <boost/context/fcontext.hpp>

typedef     boost::context::fcontext_t      fcontext_t;

static fcontext_t* parent = 0;
static fcontext_t* local  = 0;

static henson::ProcMap* procmap = 0;

int  henson_active()
{
    return (parent != 0 && local != 0);
}

void henson_yield()
{
    if (parent == 0 || local == 0)      // not running under henson; do nothing
        return;
    boost::context::jump_fcontext(local, *parent, 0);
}

void henson_set_contexts(void* p, void* l)
{
    parent = (fcontext_t*) p;
    local  = (fcontext_t*) l;
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
