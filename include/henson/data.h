#ifndef HENSON_DATA_H
#define HENSON_DATA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void        henson_set_namemap(void* nm);

void        henson_save_array(const char* name, void*  address, size_t  type, size_t  count, size_t  stride);
void        henson_load_array(const char* name, void** address, size_t* type, size_t* count, size_t* stride);

void        henson_save_pointer(const char* name, void*  ptr);
void        henson_load_pointer(const char* name, void** ptr);

void        henson_save_size_t(const char* name, size_t  x);
void        henson_load_size_t(const char* name, size_t* x);

#ifdef __cplusplus
}
#endif

#endif

