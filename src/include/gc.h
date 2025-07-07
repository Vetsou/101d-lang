#ifndef _GC_H_
#define _GC_H_

#include "object.h"

typedef struct {
    obj_t *objects;
} gc_t;

void gc_init(gc_t *gc);
void gc_free(gc_t *gc);

/* OBJ ALLOC */

obj_str_t* copy_str(gc_t *gc, const char *chars, int32_t len);
obj_str_t* take_str(gc_t *gc, char *chars, int32_t len);

#endif // _GC_H_