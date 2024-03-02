/* ********************************
 * Author:       Zhanglele
 * Description:  链表测试模块
 * create time: 2022.01.23
 ********************************/

#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "t_list.h"
#include "unittest.h"

TEST_SETUP(list_test)
{
}

TEST_TEAR_DOWN(list_test)
{
}

TEST_CASE_SETUP(list_test)
{
}

TEST_CASE_TEAR_DOWN(list_test)
{
}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static int g_values[] = {0};

int value_cmp(void *data1, void *data2)
{
    return *(int *)data1 > *(int *)data2;
}

TEST(list_test, test_basic_list)
{
    int ret;
    struct list_ops_s ops = {
        .cmp = value_cmp
    };
    list_t *list = list_create(&ops);
    ASSERT_TRUE(list != NULL);

    /* 添加链表节点 */
    for (int i = 0; i < ARRAY_SIZE(g_values); i++) {
        ret = list_add_head(list, &g_values[i], sizeof(g_values[i]));
        ASSERT_TRUE(ret == 0);
    }
    ASSERT_TRUE(list->num == ARRAY_SIZE(g_values));

    /* 删除链表节点 */
    for (int i = 0; i < ARRAY_SIZE(g_values); i++) {
        ret = list_del_head(list);
        ASSERT_TRUE(ret == 0);
    }

    ASSERT_TRUE(list->num == 0);
    list_destroy(list);
}

int main(int argc, char **argv)
{
    TEST_MAIN(argc, argv);
    return 0;
}