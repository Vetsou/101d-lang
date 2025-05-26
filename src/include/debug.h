#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "chunk.h"

void disassemble_chunk(chunk_t *chunk, const char *name);
int disassemble_instr(chunk_t *chunk, size_t offset);

#endif // _DEBUG_H_