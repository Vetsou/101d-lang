#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

typedef enum {
    RESULT_OK = 0,
    RESULT_FAIL = 1
} result_t;

#endif // _TYPES_H_