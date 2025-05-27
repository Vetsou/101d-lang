#ifndef _VALUES_H_
#define _VALUES_H_

#include "types.h"

typedef double cvalue_t;

typedef struct {
    size_t len;
    size_t capacity;
    cvalue_t *values;
} value_array_t;

void value_array_init(value_array_t *array);
void value_array_write(value_array_t *array, cvalue_t value);
void value_array_free(value_array_t *array);

void print_value(cvalue_t value);

#endif // _VALUES_H_