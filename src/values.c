#include "values.h"
#include "mem.h"

#include <stdio.h>

//
// PUBLIC
//

void value_array_init(
    value_array_t *array
) {
    *array = (value_array_t) {
        .capacity = 0,
        .len = 0,
        .values = NULL
    };
}

void value_array_write(
    value_array_t *array,
    value_t value
) {
    if (array->capacity < array->len + 1) {
        size_t prev_capacity = array->capacity;
        array->capacity = CAPACITY_GROW(prev_capacity);
        array->values = ARRAY_GROW(value_t, array->values,
            prev_capacity, array->capacity);
    }

    array->values[array->len] = value;
    array->len++;
}

void value_array_free(
    value_array_t *array
) {
  ARRAY_FREE(value_t, array->values, array->capacity);
  value_array_init(array);
}

void print_value(value_t value) {
    printf("%g", AS_NUMBER(value));
}