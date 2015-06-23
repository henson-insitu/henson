#ifndef HENSON_DATA_H
#define HENSON_DATA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void        henson_set_namemap(void* nm);
void        henson_save_array(const char* name, void* address, size_t type, size_t count, size_t stride);
void        henson_load_array(const char* name, void** address, size_t* type, size_t* count, size_t* stride);

#ifdef __cplusplus
}
#endif

#endif

