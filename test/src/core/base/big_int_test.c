/* ********************************
 * Author:       Zhanglele
 * Description:  哈希表模块测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "unittest.h"
#include "big_int.h"

long_integer_t *g_integer = NULL;

TEST_SETUP(big_int_test)
{
}

TEST_TEAR_DOWN(big_int_test)
{

}

TEST_CASE_SETUP(big_int_test)
{
    g_integer = integer_new(0, sizeof(uint32_t) / sizeof(uint8_t));
    ASSERT_TRUE(g_integer != NULL);
}

TEST_CASE_TEAR_DOWN(big_int_test)
{
    integer_free(g_integer);
    g_integer = NULL;
}

static void integer_assert(long_integer_t *integer, uint32_t val)
{
    long_integer_t *target = integer_new(val, sizeof(uint32_t) / sizeof(uint8_t));
    ASSERT_TRUE(integer_equal(integer, target));
    integer_free(target);
}

/**
 *  用例1: 测试整数赋值操作=
 */
TEST(big_int_test, test_int_set)
{
    uint32_t vals[] = {0x91, 0x791, 0x4791, 0x49171, 0x149171, 0x8149171, 0x81495171}; 

    for (int i = 0; i < sizeof(vals) / sizeof(vals[0]); i++) {
        uint32_t val = vals[i];

        // 整数设置
        integer_set(g_integer, val);

        // 整数断言
        integer_assert(g_integer, val);
    }
}

TEST_SUITE_RUNNER(big_int_test)
{
    RUN_TEST_CASE(big_int_test, test_int_set);
}

int main()
{
    RUN_TEST_SUITE(big_int_test);
    return 0;
}