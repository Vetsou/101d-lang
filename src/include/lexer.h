#ifndef _LEXER_H_
#define _LEXER_H_

#include "types.h"

typedef enum {
    TOK_EOF = 0, TOK_ERROR,

    // Symbols
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_BANG, TOK_EQUAL, TOK_GREATER, TOK_LESS,
    TOK_EQUAL_EQUAL, TOK_GREATER_EQUAL, TOK_LESS_EQUAL, TOK_UNEQUAL,
    TOK_OR, TOK_AND,
    TOK_LEFT_PAREN, TOK_RIGHT_PAREN, TOK_LEFT_BRACE, TOK_RIGHT_BRACE,
    TOK_COMMA, TOK_DOT,

    // Keywords
    TOK_IF, TOK_ELSE,
    TOK_PRINT, TOK_NULL,
    TOK_VAR, TOK_FUNCTION, TOK_RETURN,

    // Literals
    TOK_NUMBER, TOK_IDENTIFIER, TOK_STRING, TOK_TRUE, TOK_FALSE
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