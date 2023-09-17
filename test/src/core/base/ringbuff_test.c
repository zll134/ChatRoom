/* ********************************
 * Author:       Zhanglele
 * Description:  ringbuff测试模块
 * create time: 2022.01.23
 ********************************/

#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "t_list.h"
#include "unittest.h"

#include "pub_def.h"
#include "ringbuff.h"

#define MAX_NUM 4
ringbuff_t *g_ringbuf = NULL;

TEST_SETUP(ringbuff_test)
{
}

TEST_TEAR_DOWN(ringbuff_test)
{
}

TEST_CASE_SETUP(ringbuff_test)
{
    uint32_t buf_size = MAX_NUM * sizeof(uint32_t);

    ringbuff_option_t option = {
        .buff_size = buf_size
    };
    g_ringbuf = ringbuff_create(&option);;
    ASSERT_TRUE(g_ringbuf != NULL);
}

TEST_CASE_TEAR_DOWN(ringbuff_test)
{
    ringbuff_destroy(g_ringbuf);
    g_ringbuf = NULL;
}

static void test_assert_ringbuff(uint32_t *data, uint32_t len)
{
    uint32_t read_num = len < MAX_NUM ? len : MAX_NUM;

    int ret = ringbuff_write(g_ringbuf, (uint8_t *)data, len * sizeof(uint32_t));
    ASSERT_EQ(ret, TOY_OK);

    uint32_t read_data[MAX_NUM] = {0};
    int size = ringbuff_read(g_ringbuf, (uint8_t *)read_data, sizeof(read_data));
    ASSERT_TRUE(size > 0);

    for (int i = 0; i < read_num; i++) {
        ASSERT_EQ(data[len - read_num + i], read_data[i]);
    }
}

TEST(ringbuff_test, test_ringbuff_When_data_size_short)
{
    uint32_t data[] = {1, 2, 3};
    test_assert_ringbuff(data, ARRAY_SIZE(data));
}

TEST(ringbuff_test, test_ringbuff_When_data_size_long)
{
    uint32_t data[] = {1, 2, 3, 4, 5, 6, 7};
    test_assert_ringbuff(data, ARRAY_SIZE(data));
}


TEST_SUITE_RUNNER(ringbuff_test)
{
    RUN_TEST_CASE(ringbuff_test, test_ringbuff_When_data_size_short);
    RUN_TEST_CASE(ringbuff_test, test_ringbuff_When_data_size_long);
}

int main()
{
    RUN_TEST_SUITE(ringbuff_test);
    return 0;
}