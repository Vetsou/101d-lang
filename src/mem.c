#include "mem.h"

#include "err.h"
#include <stdlib.h>

void* mem_realloc(
    void *pointer,
    size_t old_size,
    size_t new_size
) {
    (void)old_size;

    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL) exit(DL_MEMORY_ALLOC_ERR);
    return result;
}