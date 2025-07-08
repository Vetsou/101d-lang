#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "types.h"
#include "values.h"

typedef enum {
    OBJ_STRING,
} obj_type_t;

struct obj_t {
    obj_type_t type;
    struct obj_t *next;
};

void print_obj(value_t value);

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

//
// STRING OBJECT
//

struct obj_str_t {
    obj_t obj;
    int32_t len;
    char chars[];
};

static inline bool is_obj_type(value_t value, obj_type_t type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#define IS_STRING(value)  is_obj_type(value, OBJ_STRING)

#define AS_STRING(value)  ((obj_str_t*)AS_OBJ(value))
#define AS_CSTRING(value) (((obj_str_t*)AS_OBJ(value))->chars)

#endif // _OBJECT_H_