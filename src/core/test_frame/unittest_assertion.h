/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框断言模块
 * create time: 2023.12.17
 ********************************/
#ifndef UNITTEST_ASSERTION_H
#define UNITTEST_ASSERTION_H

#include <stdint.h>
#include <stdbool.h>

enum {
    ASSERTION_TYPE_ASSERT, /* 检查点失败时，退出函数 */
    ASSERTION_TYPE_EXPECT, /* 检查点失败时，继续往下执行 */
    ASSERTION_TYPE_MAX
};

void unit_test_assert_boolean(bool condition, bool expected, uint32_t assert_type, 
    const char *file, uint32_t line);

void unit_test_assert_integer(uint32_t actual, uint32_t expected, uint32_t assert_type, 
    const char *file, uint32_t line);

#endif