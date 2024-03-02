/* ********************************
 * Author:       Zhanglele
 * Description:  哈希表模块测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "unittest.h"
#include "pub_def.h"
#include "log.h"
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
    uint32_t vals[] = {0x91, 0x791, 0x4791, 0x49171,
                       0x149171, 0x8149171, 0x81495171}; 

    for (int i = 0; i < ARRAY_SIZE(vals); i++) {
        uint32_t val = vals[i];

        // 整数设置
        integer_set(g_integer, val);

        // 整数断言
        integer_assert(g_integer, val);
    }
}

/**
 *  用例2: 测试整数++操作
 */
TEST(big_int_test, test_int_inc)
{
    // 随机生成测试整数
    uint32_t vals[] = {0x0, 0xbf, 0xff, 0x800e, 0xa095,
                       0xffff, 0x8aa479c6, 0xffffff, 0xffffffff}; 

    for (int i = 0; i < ARRAY_SIZE(vals); i++) {
        uint32_t val = vals[i];
        integer_set(g_integer, val);

        TRACE("case %d: val %u", i, val);

        // 长整数++
        val++;
        integer_inc(g_integer);

        integer_assert(g_integer, val);
    }
}

/**
 *  用例3: 测试整数<<操作
 */
TEST(big_int_test, test_int_shift_left)
{
    // 随机生成测试整数
    uint32_t vals[] = {0xbf, 0x800e, 0xfd1ffe, 0x12345678}; 
    uint32_t bits[] = {0, 5, 8, 10, 16, 19, 24, 27, 32, 40};
    int case_idx = 0;
    for (int i = 0; i < ARRAY_SIZE(vals); i++) {
        for (int j = 0; j < ARRAY_SIZE(bits); j++) {
            uint32_t val = vals[i];
            integer_set(g_integer, val);

            TRACE("case %d: val %u, bit %u", case_idx, val, bits[j]);

            // 长整数偏移
            val = val << bits[j];
            integer_shift_left(g_integer, bits[j]);

            integer_assert(g_integer, val);
            case_idx++;
        } 
    }
}

/**
 *  用例4: 测试整数>>操作
 */
TEST(big_int_test, test_int_shift_right)
{
    // 随机生成测试整数
    uint32_t vals[] = {0xbf, 0x800e, 0xfd1ffe, 0x12345678}; 
    uint32_t bits[] = {0, 5, 8, 10, 16, 19, 24, 27, 32, 40};
    int case_idx = 0;
    for (int i = 0; i < ARRAY_SIZE(vals); i++) {
        for (int j = 0; j < ARRAY_SIZE(bits); j++) {
            uint32_t val = vals[i];
            integer_set(g_integer, val);

            TRACE("case %d: val %u, bit %u", case_idx, val, bits[j]);
            // 长整数向右偏移
            val = val >> bits[j];
            integer_shift_right(g_integer, bits[j]);

            integer_assert(g_integer, val);
            case_idx++;
        } 
    }
}

/**
 *  用例5: 测试整数获取bit位
 */
TEST(big_int_test, test_int_get_bit)
{
    // 待测试随机整数
    uint32_t val = 0x76EC5249; // 0111 0110 1110 1100 0101 0010 0100 1001
    uint32_t indexes[] = {0, 3, 4, 10, 20, 27, 30};
    uint32_t bits[] = {1, 1, 0, 0, 0, 0, 1};
    integer_set(g_integer, val);
    for (int i = 0; i < ARRAY_SIZE(indexes); i++) {
        ASSERT_EQ(integer_get_bit(g_integer, indexes[i]), bits[i]);
    }
}

int main(int argc, char **argv)
{
    TEST_MAIN(argc, argv);
    return 0;
}