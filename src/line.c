#include "line.h"
#include "mem.h"

void line_array_init(
    line_array_t *array
) {
    *array = (line_array_t) {
        .capacity = 0,
        .len = 0,
        .line_info = NULL
    };
}

void line_array_write(
    line_array_t *array,
    int32_t line_num
) {
    if (array->len > 0 && array->line_info[array->len - 1].line == line_num) {
        array->line_info[array->len - 1].count++;
    } else {
        if (array->capacity < array->len + 1) {
            size_t prev_capacity = array->capacity;
            array->capacity = CAPACITY_GROW(prev_capacity);
            array->line_info = ARRAY_GROW(line_info_t, array->line_info,
                prev_capacity, array->capacity);
        }

        array->line_info[array->len++] = (line_info_t) {
            .line = line_num,
            .count = 1
        };
    }
}

void line_array_free(
    line_array_t *array
) {
  ARRAY_FREE(line_info_t, array->line_info, array->capacity);
  line_array_init(array);
}