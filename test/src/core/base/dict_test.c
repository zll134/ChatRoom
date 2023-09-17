/* ********************************
 * Author:       Zhanglele
 * Description:  哈希表模块测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "pub_def.h"
#include "unittest.h"
#include "dict.h"
#include "hash.h"
#include "common_data.c"

dict_t *g_dict = NULL;

static uint32_t test_hash_func(const void *key)
{
    return dict_int_hash_func(*(uint32_t *)key);
}

bool test_key_match(const void *key1, const void *key2)
{
    return *(uint32_t *)key1 == *(uint32_t *)key2;
}

TEST_SETUP(dict_test)
{
    dict_config_t config = {
        .priv_data = NULL,
        .hash_func = test_hash_func,
        .key_match = test_key_match
    };
    g_dict = dict_create(&config);
    ASSERT_TRUE(g_dict != NULL);
}

TEST_TEAR_DOWN(dict_test)
{
    dict_destroy(g_dict);
    g_dict = NULL;
}

TEST_CASE_SETUP(dict_test)
{
}

TEST_CASE_TEAR_DOWN(dict_test)
{
}

TEST(dict_test, basic_add_and_delete_expect_ok)
{
    uint32_t v = 1;

    // 添加数据
    int ret = dict_add(g_dict, &v, sizeof(uint32_t));
    ASSERT_EQ(ret, TOY_OK);

    // 删除数据
    ret = dict_delete(g_dict, &v);
    ASSERT_EQ(ret, TOY_OK);
    ASSERT_EQ(dict_get_entry_num(g_dict), 0);
}

TEST(dict_test, batch_add_and_delete_expect_ok)
{
    uint32_t *values = g_test_data1;
    uint32_t num = ARRAY_SIZE(g_test_data1);
    int ret;

    // 添加数据
    for (int i = 0; i < num; i++) {
        ret = dict_add(g_dict, &values[i], sizeof(uint32_t));
        ASSERT_EQ(ret, TOY_OK);
    }
    ASSERT_EQ(dict_get_entry_num(g_dict), num);

    // 查询数据
    for (int i = 0; i < num; i++) {
        dict_entry_t *entry = dict_find(g_dict, &values[g_test_random_index1[i]]);
        ASSERT_EQ(*(int *)entry->record, values[g_test_random_index1[i]]);
    }

    // 删除数据
    for (int i = 0; i < num; i++) {
        ret = dict_delete(g_dict, &values[g_test_random_index1[i]]);
        ASSERT_EQ(ret, TOY_OK);
    }
    ASSERT_EQ(dict_get_entry_num(g_dict), 0);

    TEST_INFO("aver search len: %d", dict_get_average_search_len(g_dict));
}

TEST_SUITE_RUNNER(dict_test)
{
    RUN_TEST_CASE(dict_test, basic_add_and_delete_expect_ok);
    RUN_TEST_CASE(dict_test, batch_add_and_delete_expect_ok);
}

int main()
{
    RUN_TEST_SUITE(dict_test);
    return 0;
}