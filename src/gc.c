#include "gc.h"

#include "mem.h"

#include <stdio.h>
#include <string.h>

//
// PRIVATE GC
//

#define ALLOC_OBJ(gc, type, obj_type) \
    (type*)obj_alloc(gc, sizeof(type), obj_type)

static obj_t* obj_alloc(
    gc_t *gc,
    size_t size,
    obj_type_t type
) {
    obj_t* object = (obj_t*)mem_realloc(NULL, 0, size);
    object->type = type;

    // Save obj ref
    object->next = gc->objects;
    gc->objects = object;

    return object;
}

static obj_str_t* str_alloc(
    gc_t *gc,
    const char *chars,
    int32_t len
) {
    size_t str_obj_size = sizeof(obj_str_t) + (len + 1) * sizeof(char);
    obj_str_t *str = (obj_str_t*)obj_alloc(gc, str_obj_size, OBJ_STRING);
    str->len = len;

    memcpy(str->chars, chars, len);
    str->chars[len] = '\0';

    return str;
}

static void obj_free(
    obj_t *obj
) {
    switch (obj->type) {
        case OBJ_STRING: {
            FREE(obj_str_t, obj);
            break;
        }
    }
}

//
// PUBLIC GC
//

void gc_init(
    gc_t *gc
) {
    gc->objects = NULL;
}

void gc_free(
    gc_t *gc
) {
    obj_t *curr = gc->objects;

    while (curr != NULL) {
        obj_t *next = curr->next;
        obj_free(curr);
        curr = next;
    }
}

obj_str_t* copy_str(
    gc_t *gc,
    const char *chars,
    int32_t len
) {
    return str_alloc(gc, chars, len);
}

obj_str_t* take_str(
    gc_t *gc,
    char *chars,
    int32_t len
) {
    obj_str_t *str = str_alloc(gc, chars, len);
    ARRAY_FREE(char, chars, len + 1);
    return str;
}