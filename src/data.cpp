#include <henson/data.h>

#include <henson/namemap.hpp>
#include <henson/io/array.hpp>


static henson::NameMap* namemap = 0;

void        henson_set_namemap(void* nm)
{
    namemap = (henson::NameMap*) nm;
}

void        henson_save_array(const char* name, void* address, size_t type, size_t count, size_t stride)
{
    if (!namemap) return;
    diy::save((*namemap)[name], henson::io::Array(address, type, count, stride));
}

void        henson_load_array(const char* name, void** address, size_t* type, size_t* count, size_t* stride)
{
    if (!namemap) return;

    henson::io::Array a;
    diy::load((*namemap)[name], a);
    *address = a.address;
    *type    = a.type;
    *count   = a.count;
    *stride  = a.stride;
}
