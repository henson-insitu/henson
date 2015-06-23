#include <henson/context.h>

#include <boost/context/fcontext.hpp>

#include <format.h>

typedef     boost::context::fcontext_t      fcontext_t;

static fcontext_t* parent;
static fcontext_t* local;

static MPI_Comm    world;

void yield()
{
    if (local == 0 || parent == 0)
        fmt::print("Warning local or parent are zero\n");
    boost::context::jump_fcontext(local, *parent, 0);
}

void set_contexts(void* p, void* l)
{
    parent = (fcontext_t*) p;
    local  = (fcontext_t*) l;
}

void set_world(MPI_Comm w)
{
    world = w;
}

MPI_Comm get_world()
{
    return world;
}
