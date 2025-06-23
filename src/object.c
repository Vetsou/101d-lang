#include "object.h"

#include "mem.h"
#include "object.h"
#include "values.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

#define ALLOC_OBJ(type, obj_type) \
    (type*)obj_alloc(sizeof(type), obj_type)

static obj_t* obj_alloc(
    size_t size,
    obj_type_t type
) {
    obj_t* object = (obj_t*)mem_realloc(NULL, 0, size);
    object->type = type;
    return object;
}

static obj_str_t* alloc_str(
    char *chars,
    int32_t len
) {
    obj_str_t *str = ALLOC_OBJ(obj_str_t, OBJ_STRING);
    str->len = len;
    str->chars = chars;
    return str;
}

obj_str_t* copy_str(
    const char *chars,
    int32_t len
) {
    char *heap_chars = ALLOC(char, len + 1);
    memcpy(heap_chars, chars, len);
    heap_chars[len] = '\0';
    return alloc_str(heap_chars, len);
}

obj_str_t* take_str(
    char *chars,
    int32_t len
) {
    return alloc_str(chars, len);
}

void print_obj(
    value_t value
) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: printf("%s", AS_CSTRING(value)); break;
    }
}