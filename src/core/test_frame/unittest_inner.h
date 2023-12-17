/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框架内部模块
 * create time: 2023.12.17
 ********************************/
#ifndef UNITTEST_INNER_H
#define UNITTEST_INNER_H

#include <stdint.h>
#include <stdbool.h>

enum {
    ASSERTION_TYPE_ASSERT, /* 检查点失败时，退出函数 */
    ASSERTION_TYPE_EXPECT, /* 检查点失败时，继续往下执行 */
    ASSERTION_TYPE_MAX
};
typedef void unit_func_t(void);

typedef struct {
    unit_func_t *setup;
    unit_func_t *test_body;
    unit_func_t *tear_down;
    const char *group;
    const char *name;
} unit_test_case_t;

typedef struct {
    unit_func_t *setup;
    unit_func_t *test_body;
    unit_func_t *tear_down;
    const char *group;
} unit_test_suite_t;

void unit_test_suite_runner(unit_test_suite_t* suite);

void unit_test_case_runner(unit_test_case_t* test_case);

void unit_test_assert_boolean(bool condition, bool expected, uint32_t assert_type, 
    const char *file, uint32_t line);

void unit_test_assert_integer(uint32_t actual, uint32_t expected, uint32_t assert_type, 
    const char *file, uint32_t line);

#endif