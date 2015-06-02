#include <henson/context.h>

#include <boost/context/fcontext.hpp>

typedef     boost::context::fcontext_t      fcontext_t;

static fcontext_t* parent;
static fcontext_t* local;

void yield()
{
    boost::context::jump_fcontext(local, *parent, 0);
}

void set_contexts(void* p, void* l)
{
    parent = (fcontext_t*) p;
    local  = (fcontext_t*) l;
}
