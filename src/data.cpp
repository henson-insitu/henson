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
    henson::Value v; v.tag = v._array; v.a = henson::Array(address, type, count, stride);
    namemap->add(name, v);
}

void
henson_load_array(const char* name, void** address, size_t* type, size_t* count, size_t* stride)
{
    if (!namemap) return;

    henson::Array a = namemap->get(name).a;
    *address = a.address;
    *type    = a.type;
    *count   = a.count;
    *stride  = a.stride;
}

void
henson_save_pointer(const char* name, void* ptr)
{
    if (!namemap) return;
    henson::Value v; v.tag = v._ptr; v.p = ptr;
    namemap->add(name, v);
}

void
henson_load_pointer(const char* name, void** ptr)
{
    if (!namemap) return;
    *ptr = namemap->get(name).p;
}

void
henson_save_size_t(const char* name, size_t  x)
{
    if (!namemap) return;
    henson::Value v; v.tag = v._size_t; v.s = x;
    namemap->add(name, v);
}

void
henson_load_size_t(const char* name, size_t* x)
{
    if (!namemap) return;
    *x = namemap->get(name).s;
}

void
henson_save_int(const char* name, int  x)
{
    if (!namemap) return;
    henson::Value v; v.tag = v._int; v.i = x;
    namemap->add(name, v);
}

void
henson_load_int(const char* name, int* x)
{
    if (!namemap) return;
    *x = namemap->get(name).i;
}

void
henson_save_float(const char* name, float  x)
{
    if (!namemap) return;
    henson::Value v; v.tag = v._float; v.f = x;
    namemap->add(name, v);
}

void
henson_load_float(const char* name, float* x)
{
    if (!namemap) return;
    *x = namemap->get(name).f;
}

void
henson_save_double(const char* name, double  x)
{
    if (!namemap) return;
    henson::Value v; v.tag = v._double; v.d = x;
    namemap->add(name, v);
}

void
henson_load_double(const char* name, double* x)
{
    if (!namemap) return;
    *x = namemap->get(name).d;
}

// C++ interface
void
henson::save(const std::string& name, Value x)
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

henson::Value
henson::load(const std::string& name)
{
    if (!get_namemap()) return Value();
    return get_namemap()->get(name);
}
