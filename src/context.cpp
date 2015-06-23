#include <henson/context.h>

#include <boost/context/fcontext.hpp>

typedef     boost::context::fcontext_t      fcontext_t;

static fcontext_t* parent = 0;
static fcontext_t* local  = 0;

static MPI_Comm    world  = MPI_COMM_WORLD;

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

void henson_set_world(MPI_Comm w)
{
    world = w;
}

MPI_Comm henson_get_world()
{
    return world;
}
