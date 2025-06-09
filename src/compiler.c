#include "compiler.h"

#include "types.h"
#include <stdio.h>

static void _error_at(
    parser_t *parser,
    token_t *tok,
    const char *msg
) {
    parser->panic_mode = true;
    parser->had_err = true;
    fprintf(stderr, "[line %d] Error", tok->line);

    if (tok->type == TOK_EOF) {
        fprintf(stderr, " at end");
    } else if (tok->type == TOK_ERROR) { /* No action */ }
    else {
        fprintf(stderr, " at '%.*s'", tok->length, tok->start);
    }

    fprintf(stderr, ": %s\n", msg);
}

static void _error_at_curr(
    parser_t *parser,
    const char *msg
) {
    _error_at(parser, &parser->prev, msg);
}

static void _compiler_advance(
    parser_t *parser
) {
    parser->prev = parser->curr;

    while (true) {
        parser->curr = lexer_scan(parser->scanner);
        if (parser->curr.type != TOK_ERROR) break;

        _error_at_curr(parser, parser->curr.start);
    }
}

static void _consume_token(
    parser_t *parser,
    token_type_t type,
    const char *msg
) {
    if (parser->curr.type == type) {
        _compiler_advance(parser);
        return;
    }

    _error_at_curr(parser, msg);
}

bool compile(
    const char *source,
    chunk_t *chunk
) {
    lexer_t lexer;
    lexer_init(&lexer, source);

    parser_t parser = (parser_t) {
        .scanner = &lexer,
        .had_err = false,
        .panic_mode = false
    };

    _compiler_advance(&parser);
    _consume_token(&parser, TOK_EOF, "Expect end.");

    return !parser.had_err;
}