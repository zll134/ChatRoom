/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框架
 * create time: 2022.4.17
 ********************************/
#ifndef UNITTEST_H
#define UNITTEST_H

#include <stdint.h>
#include <stdbool.h>
#include "unittest_print.h"
#include "unittest_inner.h"
#include "unittest_assertion.h"

/* 定义测试套准备函数 */
#define TEST_SETUP(group) \
    TEST_SUITE_INIT(group) \
    void TEST_##group##_SETUP(void)

/* 定义测试套析构函数 */
#define TEST_TEAR_DOWN(group) \
    void TEST_##group##_TEAR_DOWN(void)

/* 定义测试用例准备函数 */
#define TEST_CASE_SETUP(group) \
    void TEST_##group##_CASE_SETUP(void)

/* 定义测试用例销毁函数 */
#define TEST_CASE_TEAR_DOWN(group) \
    void TEST_##group##_CASE_TEAR_DOWN(void)

/* 定义测试用例 */
#define TEST(groupval, nameval) \
    TEST_INIT(groupval, nameval, __FILE__, __LINE__); \
    void TEST_##groupval##_##nameval##_RUN(void)

/* 运行测试用例 */
#define TEST_MAIN(argc, argv) \
    unit_test_run(argc, argv)

/* 布尔值断言 */
#define EXPECT_TRUE(condition) \
    unit_test_assert_boolean(condition, true, ASSERTION_TYPE_EXPECT, __FILE__, __LINE__);

#define EXPECT_FALSE(condition) \
    unit_test_assert_boolean(condition, false, ASSERTION_TYPE_EXPECT, __FILE__, __LINE__);

#define ASSERT_TRUE(condition) \
    unit_test_assert_boolean(condition, true, ASSERTION_TYPE_ASSERT, __FILE__, __LINE__);

#define ASSERT_FALSE(condition) \
    unit_test_assert_boolean(condition, false, ASSERTION_TYPE_ASSERT, __FILE__, __LINE__);

/* 整数断言 */
#define EXPECT_EQ(actual, expected) \
    unit_test_assert_integer((actual), (expected), ASSERTION_TYPE_EXPECT, __FILE__, __LINE__);

#define ASSERT_EQ(actual, expected) \
    unit_test_assert_integer((actual), (expected), ASSERTION_TYPE_ASSERT, __FILE__, __LINE__);

/* 打印 */
#define TEST_INFO(...) \
    do { \
        unit_print(FONT_GREEN, "[---INFO---] "); \
        unit_print(FONT_WHITE, __VA_ARGS__); \
        unit_print(FONT_WHITE, "\n"); \
    } while (0);

/* 打印 */
#define TEST_ERR(...) \
    do { \
        unit_print(FONT_RED, "[--- ERR --] "); \
        unit_print(FONT_WHITE, __VA_ARGS__); \
        unit_print(FONT_WHITE, "\n"); \
    } while (0);


#endif