#include <stdio.h>
#include "lexer.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void check_tokens(lexer_t *lexer, token_type_t *expected_types) {
    uint32_t idx = 0;
    while(true) {
        token_t token = lexer_scan(lexer);
        token_type_t expected = expected_types[idx];

        TEST_ASSERT_EQUAL_INT(expected, token.type);

        if (token.type == TOK_EOF) break;
        idx++;
    }
}

/********************
    TESTS
********************/

void lexer_test_function() {
    token_type_t expected[] = {
        TOK_FUNCTION, TOK_IDENTIFIER, TOK_LEFT_PAREN, TOK_RIGHT_PAREN,
        TOK_LEFT_BRACE, TOK_RETURN, TOK_NUMBER, TOK_RIGHT_BRACE,
        TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "FN my_func_name() [\nRET 0\n]", 28);
    check_tokens(&lexer, expected);
}

void lexer_test_if() {
    token_type_t expected[] = {
        TOK_IF, TOK_LEFT_PAREN,
        TOK_IDENTIFIER, TOK_UNEQUAL, TOK_IDENTIFIER,
        TOK_RIGHT_PAREN,
        TOK_LEFT_BRACE,
        TOK_PRINT, TOK_IDENTIFIER,
        TOK_RIGHT_BRACE, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "IF (a <> b) [\nPRINT a\n]", 24);
    check_tokens(&lexer, expected);
}

void lexer_test_variable() {
    token_type_t expected[] = {
        TOK_VAR, TOK_IDENTIFIER,
        TOK_EQUAL, TOK_NUMBER, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "VAR a = 4", 10);
    check_tokens(&lexer, expected);
}

void lexer_test_math_expression() {
    token_type_t expected[] = {
        TOK_NUMBER, TOK_PLUS, TOK_NUMBER,
        TOK_STAR, TOK_LEFT_PAREN, TOK_NUMBER,
        TOK_MINUS, TOK_NUMBER, TOK_RIGHT_PAREN,
        TOK_SLASH, TOK_NUMBER, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "3 + 7 * (2 - 3) / 3", 10);
    check_tokens(&lexer, expected);
}

int main(void) {
    printf("#################### [LEXER TESTS] ####################\n");
    UNITY_BEGIN();

    RUN_TEST(lexer_test_function);
    RUN_TEST(lexer_test_if);
    RUN_TEST(lexer_test_variable);
    RUN_TEST(lexer_test_math_expression);

    return UNITY_END();
}