#include <henson/data.h>
#include <henson/data.hpp>


static henson::NameMap* namemap = 0;

void
henson_set_namemap(void* nm)
{
    namemap = (henson::NameMap*) nm;
}

void
henson_save_array(const char* name, void* address, size_t type, size_t count, size_t stride)
{
    if (!namemap) return;
    namemap->add(name, new henson::Array(address, type, count, stride));
}

void
henson_load_array(const char* name, void** address, size_t* type, size_t* count, size_t* stride)
{
    if (!namemap) return;

    henson::Array* a = namemap->get<henson::Array>(name);
    *address = a->address;
    *type    = a->type;
    *count   = a->count;
    *stride  = a->stride;
}

void
henson_save_pointer(const char* name, void* ptr)
{
    if (!namemap) return;
    namemap->add(name, new henson::Value<void*>(ptr));
}

void
henson_load_pointer(const char* name, void** ptr)
{
    if (!namemap) return;
    *ptr = namemap->get< henson::Value<void*> >(name)->value;
}

void
henson_save_size_t(const char* name, size_t  x)
{
    if (!namemap) return;
    namemap->add(name, new henson::Value<size_t>(x));
}

void
henson_load_size_t(const char* name, size_t* x)
{
    if (!namemap) return;
    *x = namemap->get< henson::Value<size_t> >(name)->value;
}


// C++ interface
void
henson::save(const std::string& name, DataType* x)
{
    if (!namemap) return;
    namemap->add(name, x);
}

bool
henson::exists(const std::string& name)
{
    if (!namemap) return false;
    return namemap->exists(name);
}

henson::NameMap*
henson::get_namemap()
{
    return namemap;
}
