#ifndef _LINE_H_
#define _LINE_H_

#include "types.h"

typedef struct {
    int32_t line;
    int32_t count;
} line_info_t;

typedef struct {
    size_t len;
    size_t capacity;
    line_info_t *line_info;
} line_array_t;

void line_array_init(line_array_t *array);
void line_array_write(line_array_t *array, int32_t line_num);
void line_array_free(line_array_t *array);

#endif // _LINE_H_