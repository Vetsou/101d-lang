#include "compiler.h"

#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_TRACE_EXEC
#include "debug.h"
#endif // DEBUG_TRACE_EXEC

//
// Types
//

typedef struct {
    lexer_t *scanner;
    gc_t *gc;
    chunk_t *chunk;
    token_t curr;
    token_t prev;
    bool had_err;
    bool panic_mode;
} parser_t;

typedef enum {
    PREC_NONE = 0,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // OR
    PREC_AND,        // AND
    PREC_EQUALITY,   // == <>
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} precedence_t;

typedef void (*parse_fn)(parser_t*);

typedef struct {
    parse_fn prefix;
    parse_fn infix;
    precedence_t precedence;
} parse_rule_t;

// Froward oper declaration
static void _unary(parser_t *parser);
static void _expression(parser_t *parser);
static void _grouping(parser_t *parser);
static void _binary(parser_t *parser);
static void _number(parser_t *parser);
static void _literal(parser_t *parser);
static void _string(parser_t *parser);
// ==============================

parse_rule_t rules[] = {
    [TOK_LEFT_PAREN]    = {_grouping,   NULL,   PREC_NONE},
    [TOK_RIGHT_PAREN]   = {NULL,        NULL,   PREC_NONE},
    [TOK_LEFT_BRACE]    = {NULL,        NULL,   PREC_NONE}, 
    [TOK_RIGHT_BRACE]   = {NULL,        NULL,   PREC_NONE},
    [TOK_COMMA]         = {NULL,        NULL,   PREC_NONE},
    [TOK_DOT]           = {NULL,        NULL,   PREC_NONE},
    [TOK_MINUS]         = {_unary,   _binary,   PREC_TERM},
    [TOK_PLUS]          = {NULL,     _binary,   PREC_TERM},
    //[TOK_SEMICOLON]   = {NULL,        NULL,   PREC_NONE},
    [TOK_SLASH]         = {NULL,     _binary,   PREC_FACTOR},
    [TOK_STAR]          = {NULL,     _binary,   PREC_FACTOR},
    [TOK_BANG]          = {_unary,      NULL,   PREC_NONE},
    [TOK_UNEQUAL]       = {NULL,     _binary,   PREC_EQUALITY},
    [TOK_EQUAL]         = {NULL,        NULL,   PREC_NONE},
    [TOK_EQUAL_EQUAL]   = {NULL,     _binary,   PREC_EQUALITY},
    [TOK_GREATER]       = {NULL,     _binary,   PREC_COMPARISON},
    [TOK_GREATER_EQUAL] = {NULL,     _binary,   PREC_COMPARISON},
    [TOK_LESS]          = {NULL,     _binary,   PREC_COMPARISON},
    [TOK_LESS_EQUAL]    = {NULL,     _binary,   PREC_COMPARISON},
    [TOK_IDENTIFIER]    = {NULL,        NULL,   PREC_NONE},
    [TOK_STRING]        = {_string,     NULL,   PREC_NONE},
    [TOK_NUMBER]        = {_number,     NULL,   PREC_NONE},
    [TOK_AND]           = {NULL,        NULL,   PREC_NONE},
    //[TOK_CLASS]       = {NULL,        NULL,   PREC_NONE},
    [TOK_ELSE]          = {NULL,        NULL,   PREC_NONE},
    [TOK_FALSE]         = {_literal,    NULL,   PREC_NONE},
    //[TOK_FOR]         = {NULL,        NULL,   PREC_NONE},
    //[TOK_FUN]         = {NULL,        NULL,   PREC_NONE},
    [TOK_IF]            = {NULL,        NULL,   PREC_NONE},
    [TOK_NIL]           = {_literal,    NULL,   PREC_NONE},
    [TOK_OR]            = {NULL,        NULL,   PREC_NONE},
    [TOK_PRINT]         = {NULL,        NULL,   PREC_NONE},
    [TOK_RETURN]        = {NULL,        NULL,   PREC_NONE},
    //[TOK_SUPER]       = {NULL,        NULL,   PREC_NONE},
    //[TOK_THIS]        = {NULL,        NULL,   PREC_NONE},
    [TOK_TRUE]          = {_literal,    NULL,   PREC_NONE},
    [TOK_VAR]           = {NULL,        NULL,   PREC_NONE},
    [TOK_WHILE]         = {NULL,        NULL,   PREC_NONE},
    [TOK_ERROR]         = {NULL,        NULL,   PREC_NONE},
    [TOK_EOF]           = {NULL,        NULL,   PREC_NONE},
};

static parse_rule_t* _get_rule(
    token_type_t type
) {
    return &rules[type];
}

//
// Errors
//

static void _error_at(
    parser_t *parser,
    token_t *tok,
    const char *msg
) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    parser->had_err = true;
    fprintf(stderr, "[line %d] Error", tok->line);

    if (tok->type == TOK_EOF) {
        fprintf(stderr, " at end");
    } else if (tok->type == TOK_ERROR) {
        /* No action */
    } else {
        fprintf(stderr, " at '%.*s'", tok->length, tok->start);
    }

    fprintf(stderr, ": %s\n", msg);
}

static void _error_at_prev(
    parser_t *parser,
    const char *msg
) {
    _error_at(parser, &parser->prev, msg);
}

static void _error_at_curr(
    parser_t *parser,
    const char *msg
) {
    _error_at(parser, &parser->curr, msg);
}

//
// Byte emit
//

static void _emit_byte(
    parser_t *parser,
    uint8_t byte
) {
    chunk_write_instr(parser->chunk, byte, parser->prev.line);
}

static void _emit_bytes(
    parser_t *parser,
    uint8_t byte1,
    uint8_t byte2
) {
    _emit_byte(parser, byte1);
    _emit_byte(parser, byte2);
}

static void _emit_return(
    parser_t *parser
) {
    _emit_byte(parser, OP_RETURN);
}

static uint16_t _emit_const(
    parser_t *parser,
    value_t value
) {
    int32_t const_idx = chunk_write_const(parser->chunk, value, parser->prev.line);
    if (const_idx == -1) {
        _error_at_prev(parser, "Too many constants in one chunk.");
        return 0;
    }

    return (uint16_t)const_idx;
}

//
// Compiler utils
//

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

//
// Operations
//

static void _parse_precedence(
    parser_t *parser,
    precedence_t precedence
) {
    _compiler_advance(parser);
    parse_fn prefix_rule = _get_rule(parser->prev.type)->prefix;
    if (prefix_rule == NULL) {
        _error_at_prev(parser, "Expect expression.");
        return;
    }

    prefix_rule(parser);

    while (precedence <= _get_rule(parser->curr.type)->precedence) {
        _compiler_advance(parser);
        parse_fn infix_rule = _get_rule(parser->prev.type)->infix;
        infix_rule(parser);
    }
}

static void _number(
    parser_t *parser
) {
    double value = strtod(parser->prev.start, NULL);
    _emit_const(parser, NUMBER_VAL(value));
}

static void _string(
    parser_t *parser
) {
    _emit_const(parser, OBJ_VAL(
        copy_str(parser->gc, parser->prev.start + 1, parser->prev.length - 2)
    ));
}

static void _expression(
    parser_t *parser
) {
    _parse_precedence(parser, PREC_ASSIGNMENT);
}

static void _grouping(
    parser_t *parser
) {
    _expression(parser);
    _consume_token(parser, TOK_RIGHT_PAREN, "Expect ')' after expression");
}

static void _unary(
    parser_t *parser
) {
    token_type_t oper_type = parser->prev.type;

    _parse_precedence(parser, PREC_UNARY);

    switch (oper_type) {
        case TOK_MINUS: _emit_byte(parser, OP_NEGATE); break;
        case TOK_BANG:  _emit_byte(parser, OP_NOT); break;
        default: return; // Unreachable
    }
}

static void _literal(
    parser_t *parser
) {
    switch (parser->prev.type) {
        case TOK_FALSE: _emit_byte(parser, OP_FALSE); break;
        case TOK_TRUE:  _emit_byte(parser, OP_TRUE); break;
        case TOK_NIL:   _emit_byte(parser, OP_NIL); break;
        default: return; // Unreachable
    }
}

static void _binary(
    parser_t *parser
) {
    token_type_t oper_type = parser->prev.type;
    parse_rule_t *rule = _get_rule(oper_type);
    _parse_precedence(parser, (precedence_t)(rule->precedence + 1));

    switch (oper_type) {
        case TOK_PLUS:  _emit_byte(parser, OP_ADD);      break;
        case TOK_MINUS: _emit_byte(parser, OP_SUBTRACT); break;
        case TOK_STAR:  _emit_byte(parser, OP_MULTIPLY); break;
        case TOK_SLASH: _emit_byte(parser, OP_DIVIDE);   break;

        case TOK_UNEQUAL:       _emit_byte(parser, OP_UNEQUAL);          break;
        case TOK_EQUAL_EQUAL:   _emit_byte(parser, OP_EQUAL);            break;
        case TOK_GREATER:       _emit_byte(parser, OP_GREATER);          break;
        case TOK_GREATER_EQUAL: _emit_byte(parser, OP_GREATER_EQUAL);    break;
        case TOK_LESS:          _emit_byte(parser, OP_LESS);             break;
        case TOK_LESS_EQUAL:    _emit_byte(parser, OP_LESS_EQUAL);      break;
        default: return; // Unreachable.
    }
}

//
// Public
//

bool compile(
    gc_t *gc,
    const char *source,
    chunk_t *chunk
) {
    lexer_t lexer;
    lexer_init(&lexer, source);

    parser_t parser = (parser_t) {
        .scanner = &lexer,
        .gc = gc,
        .chunk = chunk,
        .had_err = false,
        .panic_mode = false
    };

    _compiler_advance(&parser);
    _expression(&parser);
    _consume_token(&parser, TOK_EOF, "Expect end.");
    _emit_return(&parser);

#ifdef DEBUG_TRACE_EXEC
    if (!parser.had_err) {
        disassemble_chunk(chunk, "Output code");
    }
#endif // DEBUG_TRACE_EXEC

    return !parser.had_err;
}