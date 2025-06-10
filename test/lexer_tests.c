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

void test_partial_keywords() {
    token_type_t expected[] = {
        TOK_IDENTIFIER, TOK_IDENTIFIER, TOK_IDENTIFIER, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "FNmy VARiable ELSEs");
    check_tokens(&lexer, expected);
}

void test_function() {
    token_type_t expected[] = {
        TOK_FUNCTION, TOK_IDENTIFIER, TOK_LEFT_PAREN, TOK_RIGHT_PAREN,
        TOK_LEFT_BRACE, TOK_RETURN, TOK_NUMBER, TOK_SEMICOLON, TOK_RIGHT_BRACE,
        TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "FN my_func_name() [\nRET 0;\n]");
    check_tokens(&lexer, expected);
}

void test_if() {
    token_type_t expected[] = {
        TOK_IF, TOK_LEFT_PAREN,
        TOK_IDENTIFIER, TOK_UNEQUAL, TOK_IDENTIFIER,
        TOK_RIGHT_PAREN,
        TOK_LEFT_BRACE,
        TOK_PRINT, TOK_IDENTIFIER, TOK_SEMICOLON,
        TOK_RIGHT_BRACE, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "IF (a <> b) [\nPRINT a;\n]");
    check_tokens(&lexer, expected);
}

void test_variable() {
    token_type_t expected[] = {
        TOK_VAR, TOK_IDENTIFIER,
        TOK_EQUAL, TOK_NUMBER, TOK_SEMICOLON, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "VAR a = 4;");
    check_tokens(&lexer, expected);
}

void test_math_expression() {
    token_type_t expected[] = {
        TOK_NUMBER, TOK_PLUS, TOK_NUMBER,
        TOK_STAR, TOK_LEFT_PAREN, TOK_NUMBER,
        TOK_MINUS, TOK_NUMBER, TOK_RIGHT_PAREN,
        TOK_SLASH, TOK_NUMBER, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "3 + 7 * (2 - 3) / 3");
    check_tokens(&lexer, expected);
}

void test_strings() {
    token_type_t expected[] = {
        TOK_STRING, TOK_STRING, TOK_STRING, TOK_STRING,
        TOK_EOF
    };

    lexer_t lexer;
    lexer_init(
        &lexer,
        "\"normal string\"\n"
        "\"string with # comment\"\n"
        "\"string with #> comment <# and #>#>works<#<#\"\n"
        "\"string with keywords IF VAR > < + *\""
    );

    check_tokens(&lexer, expected);
}

void test_comment() {
    token_type_t expected[] = { TOK_NUMBER, TOK_EOF };

    lexer_t lexer;
    lexer_init(&lexer, "# Some comment\n123");
    check_tokens(&lexer, expected);
}

void test_block_comment() {
    token_type_t expected[] = { TOK_VAR, TOK_EOF };

    lexer_t lexer;
    lexer_init(&lexer, "#> \nBlock\ncomment\n123\n <# VAR");
    check_tokens(&lexer, expected);
}

void test_multiple_block_comments() {
    token_type_t expected[] = { TOK_IF, TOK_EOF };

    lexer_t lexer;
    lexer_init(
        &lexer,
        "#>\n"
        "#> Test comment <#\n"
        "#>#>Double comment<#<#\n"
        "<<>> end<# IF"
    );
    check_tokens(&lexer, expected);
}

void test_while() {
    token_type_t expected[] = {
        TOK_WHILE, TOK_LEFT_PAREN,
        TOK_IDENTIFIER, TOK_UNEQUAL, TOK_NUMBER,
        TOK_RIGHT_PAREN, TOK_LEFT_BRACE,
        TOK_IDENTIFIER, TOK_EQUAL, TOK_IDENTIFIER,
        TOK_PLUS, TOK_NUMBER, TOK_SEMICOLON,
        TOK_RIGHT_BRACE, TOK_EOF
    };

    lexer_t lexer;
    lexer_init(&lexer, "WHILE (a <> 0) [\n a = a + 1;\n]");
    check_tokens(&lexer, expected);
}

int main(void) {
    printf("#################### [LEXER TESTS] ####################\n");
    UNITY_BEGIN();

    RUN_TEST(test_partial_keywords);
    RUN_TEST(test_function);
    RUN_TEST(test_if);
    RUN_TEST(test_variable);
    RUN_TEST(test_math_expression);
    RUN_TEST(test_strings);
    RUN_TEST(test_comment);
    RUN_TEST(test_block_comment);
    RUN_TEST(test_multiple_block_comments);
    RUN_TEST(test_while);

    return UNITY_END();
}