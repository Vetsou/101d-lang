#ifndef _VALUES_H_
#define _VALUES_H_

#include "types.h"

// Forward declaration object.h
typedef struct obj_t obj_t;
typedef struct obj_str_t obj_str_t;
// Forward declaration END

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ
} value_type_t;

typedef struct {
    value_type_t type;
    union {
        bool boolean;
        double number;
        obj_t *obj;
    } as;
} value_t;

bool values_equal(value_t a, value_t b);

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)
#define AS_OBJ(value)     ((value).as.obj)

#define BOOL_VAL(value)   ((value_t){VAL_BOOL, {.boolean = value}})
#define NIL_VAL           ((value_t){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((value_t){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)   ((value_t){VAL_OBJ, {.obj = (obj_t*)object}})

typedef struct {
    size_t len;
    size_t capacity;
    value_t *values;
} value_array_t;

void value_array_init(value_array_t *array);
void value_array_write(value_array_t *array, value_t value);
void value_array_free(value_array_t *array);

void print_value(value_t value);

#endif // _VALUES_H_