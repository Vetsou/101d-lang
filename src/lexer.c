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

static inline char _advance(
    lexer_t *lexer
) {
    lexer->curr++;
    return lexer->curr[-1];
}

static inline bool _is_file_end(
    lexer_t *lexer
) {
    return *lexer->curr == '\0';
}

static bool _match_and_move(
    lexer_t *lexer,
    char expected
) {
    if (_is_file_end(lexer)) return false;
    if (*lexer->curr != expected) return false;

    lexer->curr++;
    return true;
}

static void _handle_block_comment(
    lexer_t *lexer
) {
    while (!_is_file_end(lexer)) {
        switch (*lexer->curr) {
            case '\n': lexer->curr_line++; break;
            case '<':
                lexer->curr++;
                if (_match_and_move(lexer, '#')) return;
                break;
        }
        lexer->curr++;
    }
}

static void _handle_comment(
    lexer_t *lexer
) {
    while (*lexer->curr != '\n' && !_is_file_end(lexer)) {
        lexer->curr++;
    }
}

static void _skip_whitespace(
    lexer_t *lexer
) {
    while(true) {
        switch(*lexer->curr) {
            case ' ': case '\r': case '\t':
                lexer->curr++;
                break;
            case '\n':
                lexer->curr++;
                lexer->curr_line++;
                break;
            case '#':
                lexer->curr++;
                if (_match_and_move(lexer, '>')) _handle_block_comment(lexer);
                else _handle_comment(lexer);
                break;
            default: return;
        }
    }
} 

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
    _skip_whitespace(lexer);

    lexer->start = lexer->curr;
    if (_is_file_end(lexer)) return token_create(TOK_EOF, lexer);

    char curr_c = _advance(lexer);
    switch (curr_c) {
        case '+': return token_create(TOK_PLUS, lexer);
        case '-': return token_create(TOK_MINUS, lexer);
        case '/': return token_create(TOK_SLASH, lexer);
        case '*': return token_create(TOK_STAR, lexer);
        case '!':
            return token_create(
                _match_and_move(lexer, '=') ? TOK_BANG_EQUAL : TOK_BANG, lexer);
        case '=':
            return token_create(
                _match_and_move(lexer, '=') ? TOK_EQUAL_EQUAL : TOK_EQUAL, lexer);
        case '>':
            return token_create(
                _match_and_move(lexer, '=') ? TOK_GREATER_EQUAL : TOK_GREATER, lexer);
        case '<':
            return token_create(
                _match_and_move(lexer, '=') ? TOK_LESS_EQUAL : TOK_LESS, lexer);
    }

    return token_error(lexer->curr_line, "Unexpected character.");
}