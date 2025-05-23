#include "lexer.h"
#include <string.h>

//
// PRIVATE TOKEN
//

static token_t _token_create(
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

static token_t _token_error(
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

static inline bool _is_digit(
    char c
) {
    return c >= '0' && c <= '9';
}

static inline bool _is_ascii(
    char c
) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

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

static inline char _peek_next(
    lexer_t *lexer
) {
    if (_is_file_end(lexer)) return '\0';
    return lexer->curr[1];
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

static token_t _handle_number(
    lexer_t *lexer
) {
    while (_is_digit(*lexer->curr)) lexer->curr++;

    if (*lexer->curr == '.' && _is_digit(_peek_next(lexer))) {
        lexer->curr++;
        while (_is_digit(*lexer->curr)) lexer->curr++;
    }

    return _token_create(TOK_NUMBER, lexer);
}

static token_t _handle_string(
    lexer_t *lexer
) {
    while (*lexer->curr != '"' && !_is_file_end(lexer)) {
        if (*lexer->curr == '\n') lexer->curr_line++;
        lexer->curr++;
    }

    if (_is_file_end(lexer)) return _token_error(lexer->curr_line, "Unterminated string");

    // Skip closing quote
    lexer->curr++;
    return _token_create(TOK_STRING, lexer);
}

static token_type_t _handle_keyword(
    lexer_t *lexer,
    int start,
    int length,
    const char *rest,
    token_type_t type
) {
    if (lexer->curr - lexer->start == start + length &&
        memcmp(lexer->start + start, rest, length) == 0
    ) {
        return type;
    }
    return TOK_IDENTIFIER;
}

static token_type_t _get_identifier_type(
    lexer_t *lexer
) {
    // Handle keywords
    switch (lexer->start[0]) {
        case 'V': return _handle_keyword(lexer, 1, 2, "AR", TOK_VAR);
        case 'P': return _handle_keyword(lexer, 1, 4, "RINT", TOK_PRINT);
        case 'I': return _handle_keyword(lexer, 1, 1, "F", TOK_IF);
        case 'F':
            if (lexer->curr - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'N': return TOK_FUNCTION;
                    case 'A': return _handle_keyword(lexer, 2, 3, "LSE", TOK_FALSE);
                }
            }
            break;
        case 'R': return _handle_keyword(lexer, 1, 2, "ET", TOK_RETURN);
        case 'N': return _handle_keyword(lexer, 1, 2, "IL", TOK_NULL);
        case 'E': return _handle_keyword(lexer, 1, 3, "LSE", TOK_ELSE);
        case 'T': return _handle_keyword(lexer, 1, 3, "RUE", TOK_TRUE);
    }

    // Not a keyword so handle identifier
    return TOK_IDENTIFIER;
}

static token_t _handle_identifier(
    lexer_t *lexer
) {
    while (_is_ascii(*lexer->curr) || _is_digit(*lexer->curr)) lexer->curr++;
    token_type_t tok_type = _get_identifier_type(lexer);
    return _token_create(tok_type, lexer);
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
    if (_is_file_end(lexer)) return _token_create(TOK_EOF, lexer);

    char curr_c = _advance(lexer);

    if (_is_digit(curr_c)) return _handle_number(lexer);
    if (_is_ascii(curr_c)) return _handle_identifier(lexer);

    switch (curr_c) {
        case '+': return _token_create(TOK_PLUS, lexer);
        case '-': return _token_create(TOK_MINUS, lexer);
        case '/': return _token_create(TOK_SLASH, lexer);
        case '*': return _token_create(TOK_STAR, lexer);
        case '(': return _token_create(TOK_LEFT_PAREN, lexer);
        case ')': return _token_create(TOK_RIGHT_PAREN, lexer);
        case '[': return _token_create(TOK_LEFT_BRACE, lexer);
        case ']': return _token_create(TOK_RIGHT_BRACE, lexer);
        case '!': return _token_create(TOK_BANG, lexer);
        case ',': return _token_create(TOK_COMMA, lexer);
        case '.': return _token_create(TOK_DOT, lexer);
        case '|':
            if (_match_and_move(lexer, '|')) return _token_create(TOK_OR, lexer);
            break;
        case '&':
            if (_match_and_move(lexer, '&')) return _token_create(TOK_AND, lexer);
            break;
        case '=':
            return _token_create(
                _match_and_move(lexer, '=') ? TOK_EQUAL_EQUAL : TOK_EQUAL, lexer);
        case '>':
            return _token_create(
                _match_and_move(lexer, '=') ? TOK_GREATER_EQUAL : TOK_GREATER, lexer);
        case '<':
            if (_match_and_move(lexer, '=')) return _token_create(TOK_LESS_EQUAL, lexer);
            else if (_match_and_move(lexer, '>')) return _token_create(TOK_UNEQUAL, lexer);
            return _token_create(TOK_LESS, lexer);

        // Handle string
        case '"': return _handle_string(lexer);
    }

    return _token_error(lexer->curr_line, "Unexpected character.");
}