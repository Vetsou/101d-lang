#include "lexer.h"
#include <string.h>

//
// PRIVATE TOKEN
//

static token_t token_create(
    token_type_t t_type,
    lexer_t *lexer
) {
    token_t tok = {
        .type = t_type,
        .start = lexer->start,
        .length = (int32_t)(lexer->curr - lexer->start),
        .line = lexer->curr_line
    };

    return tok;
}

static token_t token_error(
    int32_t line,
    const char *msg
) {
    token_t err_tok = {
        .type = TOK_ERROR,
        .line = line,
        .start = msg,
        .length = (int32_t)strlen(msg)
    };

    return err_tok;
}

//
// PRIVATE LEXER
//

//
// PUBLIC LEXER
//

void lexer_init(
    lexer_t *lexer,
    char *chunk,
    size_t len
) {
    *lexer = (lexer_t) {
        .curr = chunk,
        .start = chunk,
        .chunk = chunk,
        .chunk_len = len,
        .curr_line = 1
    };
}

token_t lexer_scan(
    lexer_t *lexer
) {
    if (lexer->chunk_len > 1) return token_create(TOK_EOF, lexer);
    return token_error(lexer->curr_line, "Unexpected character.");
}