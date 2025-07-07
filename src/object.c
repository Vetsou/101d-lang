#include "object.h"
#include <stdio.h>

void print_obj(
    value_t value
) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: printf("%s", AS_CSTRING(value)); break;
    }
}