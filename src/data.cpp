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
    henson::Value v = henson::Array(address, type, count, stride);
    namemap->add(name, v);
}

void
henson_load_array(const char* name, void** address, size_t* type, size_t* count, size_t* stride)
{
    if (!namemap) return;

    henson::Array a = mpark::get<henson::Array>(namemap->get(name));
    *address = a.address;
    *type    = a.type;
    *count   = a.count;
    *stride  = a.stride;
}

void
henson_save_pointer(const char* name, void* ptr)
{
    if (!namemap) return;
    henson::Value v = ptr;
    namemap->add(name, v);
}

void
henson_load_pointer(const char* name, void** ptr)
{
    if (!namemap) return;
    *ptr = mpark::get<void*>(namemap->get(name));
}

void
henson_save_size_t(const char* name, size_t  x)
{
    if (!namemap) return;
    henson::Value v = x;
    namemap->add(name, v);
}

void
henson_load_size_t(const char* name, size_t* x)
{
    if (!namemap) return;
    *x = mpark::get<size_t>(namemap->get(name));
}

void
henson_save_int(const char* name, int  x)
{
    if (!namemap) return;
    henson::Value v = x;
    namemap->add(name, v);
}

void
henson_load_int(const char* name, int* x)
{
    if (!namemap) return;
    *x = mpark::get<int>(namemap->get(name));
}

void
henson_save_float(const char* name, float  x)
{
    if (!namemap) return;
    henson::Value v = x;
    namemap->add(name, v);
}

void
henson_load_float(const char* name, float* x)
{
    if (!namemap) return;
    *x = mpark::get<float>(namemap->get(name));
}

void
henson_save_double(const char* name, double  x)
{
    if (!namemap) return;
    henson::Value v = x;
    namemap->add(name, v);
}

void
henson_load_double(const char* name, double* x)
{
    if (!namemap) return;
    *x = mpark::get<double>(namemap->get(name));
}

void
henson_create_queue(const char* name)
{
    if (!namemap) return;
    namemap->create_queue(name);
}

int
henson_queue_empty(const char* name)
{
    if (!namemap) return 1;
    return namemap->queue_empty(name);
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
