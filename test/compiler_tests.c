#include <stdio.h>
#include "vm.h"
#include "unity.h"

vm_t vm;
void setUp(void)    { vm_init(&vm); }
void tearDown(void) { vm_free(&vm); }

void test_simple_add_expr() {
    dl_result_t result = vm_interpret(&vm, "2 + 3");
    TEST_ASSERT_EQUAL_INT(DL_OK, result);
}

void test_simple_bool_expr() {
    dl_result_t result = vm_interpret(&vm, "2 + 4 >= 3 - 1");
    TEST_ASSERT_EQUAL_INT(DL_OK, result);
}

void test_simple_arithmetic_expr() {
    dl_result_t result = vm_interpret(&vm, "2 + 3 * 5 - 3 / 2");
    TEST_ASSERT_EQUAL_INT(DL_OK, result);
}

void test_mixed_expr() {
    dl_result_t result = vm_interpret(&vm, "!(2 + 5 > 10) == !NIL");
    TEST_ASSERT_EQUAL_INT(DL_OK, result);
}

void test_simple_string_expr() {
    dl_result_t result = vm_interpret(&vm, "\"STR\" + \"ING\"");
    TEST_ASSERT_EQUAL_INT(DL_OK, result);
}

int main(void) {
    printf("#################### [COMPILER TESTS] ####################\n");
    UNITY_BEGIN();

    RUN_TEST(test_simple_add_expr);
    RUN_TEST(test_simple_bool_expr);
    RUN_TEST(test_simple_arithmetic_expr);
    RUN_TEST(test_mixed_expr);
    RUN_TEST(test_simple_string_expr);

    return UNITY_END();
}