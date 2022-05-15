/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框架
 * create time: 2022.4.17
 ********************************/
#ifndef UNITTEST_H
#define UNITTEST_H

#include <stdint.h>

enum {
    UNIT_TEST_TYPE_SETUP = 0,
    UNIT_TEST_TYPE_RUN,
    UNIT_TEST_TYPE_TEARDOWN
};

#define unit_test(func) { #func, func, UNIT_TEST_TYPE_RUN }

typedef struct {
    const char *name;
    void (*func)();
    int func_type;
} unit_test_t;

void assert_ne(uint64_t left, uint64_t right);
void assert_eq(uint64_t left, uint64_t right);

void run_tests(unit_test_t *tests, int len);

#endif