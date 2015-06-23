#ifndef HENSON_IO_ARRAY_H
#define HENSON_IO_ARRAY_H

#include <string>

namespace henson
{
namespace io
{

struct Array
{
            Array(void* address_ = 0, size_t type_ = 0, size_t count_ = 0, size_t stride_ = 0):
                address(address_), type(type_), count(count_), stride(stride_)      {}

    void*   address;
    size_t  type;
    size_t  count;
    size_t  stride;
};

}
}

#endif
