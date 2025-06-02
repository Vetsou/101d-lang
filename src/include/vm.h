#ifndef _VM_H_
#define _VM_H_

#include "chunk.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} interpret_result_t;

typedef struct {
    chunk_t *chunk;
    uint8_t *b_ptr;
} vm_t;

void vm_init(vm_t *vm);
interpret_result_t vm_interpret(vm_t *vm, chunk_t *chunk);
void vm_free(vm_t *vm);

#endif // _VM_H_