#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "chunk.h"
#include "lexer.h"
#include "gc.h"

bool compile(gc_t *gc, const char *source, chunk_t *chunk);

#endif //  _COMPILER_H_