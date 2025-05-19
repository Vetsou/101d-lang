#ifndef _LEXER_H_
#define _LEXER_H_

#include "types.h"

typedef enum {
    TOK_EOF = 0, TOK_ERROR,

    // Symbols
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_BANG, TOK_EQUAL, TOK_GREATER, TOK_LESS,
    TOK_BANG_EQUAL, TOK_EQUAL_EQUAL, TOK_GREATER_EQUAL, TOK_LESS_EQUAL

} token_type_t;

typedef struct {
    token_type_t type;
    const char *start;
    int32_t length;
    int32_t line;
} token_t;

typedef struct {
    char *curr;
    char *start;
    char *chunk;
    size_t chunk_len;
    int32_t curr_line;
} lexer_t;

void lexer_init(lexer_t *lexer, char *chunk, size_t len);
token_t lexer_scan(lexer_t *lexer);

#endif // _LEXER_H_