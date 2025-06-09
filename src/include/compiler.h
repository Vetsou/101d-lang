#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "chunk.h"
#include "lexer.h"

typedef struct {
    lexer_t *scanner;
    token_t curr;
    token_t prev;
    bool had_err;
    bool panic_mode;
} parser_t;

bool compile(const char *source, chunk_t *chunk);

#endif //  _COMPILER_H_