#ifndef HENSON_CONTEXT_H
#define HENSON_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

void yield();
void set_contexts(void* parent, void* local);

#ifdef __cplusplus
}
#endif

#endif
