#include <stdio.h>
#include "chunk.h"
#include "unity.h"

chunk_t chunk;
void setUp(void) { chunk_init(&chunk); }
void tearDown(void) { chunk_free(&chunk); }

/********************
    TESTS
********************/

void chunk_test_lines() {
    chunk_write_const(&chunk, NUMBER_VAL(123), 3);
    chunk_write_const(&chunk, NUMBER_VAL(442), 3);
    chunk_write_const(&chunk, NUMBER_VAL(333), 4);
    chunk_write_instr(&chunk, OP_RETURN, 5);
    chunk_write_instr(&chunk, OP_RETURN, 5);
    chunk_write_instr(&chunk, OP_RETURN, 6);

    TEST_ASSERT_EQUAL_INT(4, chunk.lines.len);
    TEST_ASSERT_EQUAL_INT(3, chunk.lines.line_info[0].line);
    TEST_ASSERT_EQUAL_INT(2, chunk.lines.line_info[0].count);

    TEST_ASSERT_EQUAL_INT(4, chunk.lines.line_info[1].line);
    TEST_ASSERT_EQUAL_INT(1, chunk.lines.line_info[1].count);

    TEST_ASSERT_EQUAL_INT(5, chunk.lines.line_info[2].line);
    TEST_ASSERT_EQUAL_INT(2, chunk.lines.line_info[2].count);

    TEST_ASSERT_EQUAL_INT(6, chunk.lines.line_info[3].line);
    TEST_ASSERT_EQUAL_INT(1, chunk.lines.line_info[3].count);
}

void chunk_test_get_line() {
    chunk_write_const(&chunk, NUMBER_VAL(123), 3);
    chunk_write_const(&chunk, NUMBER_VAL(2233), 4);
    chunk_write_instr(&chunk, OP_RETURN, 5);
    chunk_write_instr(&chunk, OP_RETURN, 5);
    chunk_write_instr(&chunk, OP_RETURN, 6);

    TEST_ASSERT_EQUAL_INT(5, chunk_get_line(&chunk, 3));
}

void chunk_test_const() {
    chunk_write_const(&chunk, NUMBER_VAL(1.55), 3);
    chunk_write_const(&chunk, NUMBER_VAL(1024), 5);

    TEST_ASSERT_EQUAL_INT(6, chunk.len); // 3 bytes per OP_CONST
    TEST_ASSERT_EQUAL_INT(2, chunk.lines.len);

    TEST_ASSERT_EQUAL_INT(2, chunk.consts.len);
    TEST_ASSERT_EQUAL_INT(1.55, AS_NUMBER(chunk.consts.values[0]));
    TEST_ASSERT_EQUAL_INT(1024, AS_NUMBER(chunk.consts.values[1]));
}

int main(void) {
    printf("#################### [BYTE CHUNK TESTS] ####################\n");
    UNITY_BEGIN();

    RUN_TEST(chunk_test_lines);
    RUN_TEST(chunk_test_get_line);
    RUN_TEST(chunk_test_const);

    return UNITY_END();
}