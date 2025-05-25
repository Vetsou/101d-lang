#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"

#define MIN_ALLOC_SIZE 16

#define CAPACITY_GROW(capacity) \
    ((capacity) < MIN_ALLOC_SIZE ? MIN_ALLOC_SIZE : (capacity) * 2)

#define ARRAY_GROW(type, pointer, old_count, new_count)     \
    (type*)mem_realloc(pointer, sizeof(type) * (old_count), \
        sizeof(type) * (new_count))

#define ARRAY_FREE(type, pointer, old_count) \
    mem_realloc(pointer, sizeof(type) * (old_count), 0)

void* mem_realloc(void *pointer, size_t old_size, size_t new_size);

#endif // _MEMORY_H_