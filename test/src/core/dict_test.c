/* ********************************
 * Author:       Zhanglele
 * Description:  哈希表模块测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "unittest.h"
#include "dict.h"

dict_t *g_dict = NULL;

TEST_SETUP(dict_test)
{
    dict_config_t config = {

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

static uint32_t g_test_vals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 

}
static void values_shuffle(int *values, int len)
{
    srand(time(NULL));
    for (int i = 0; i < len; i++) {
        int index = rand() % len;
        int t = values[0];
        values[0] = values[index];
        values[index] = t;
    }
}

TEST(dict_test, random_add_and_delete_expect_true)
{
    
}

TEST_SUITE_RUNNER(dict_test)
{
    RUN_TEST_CASE(dict_test, test_random_add_and_delete);
}

int main()
{
    RUN_TEST_SUITE(dict_test);
    return 0;
}