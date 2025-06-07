#include "compiler.h"

#include "types.h"
#include "lexer.h"

void compile(const char *source) {
    lexer_t lexer;
    lexer_init(&lexer, source);

    while (true) {
        token_t token = lexer_scan(&lexer);

        if (token.type == TOK_EOF) break;
    }
}