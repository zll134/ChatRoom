/* ********************************
 * Author:       Zhanglele
 * Description:  红黑树功能测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "rbtree.h"

#define MAX_SIZE 20
static int value_cmp(void *data1, void *data2)
{
    return *(int *)data1 - *(int *)data2;
}

static void *value_dup(void *data)
{
    int *data_dup = (int *)malloc(sizeof(int));
    *data_dup = *(int *)data;
    return data_dup;
}

static char g_dump_str[32] = {0};
static const char *value_dump(void *data)
{
    int value = *(int *)data;
    (void)sprintf(g_dump_str, "%d", value);
    return g_dump_str;
}

static void values_init(int *values, int len)
{
    for (int i = 0; i < len; i++) {
        values[i] = i;
    }
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

static void values_insert(rbtree_t *tree, int *values, int len)
{
    diag_info("start insert node");
    for (int i = 0; i < len; i++) {
        rbtree_insert(tree, (void *)&values[i]);
        rbtree_dump(tree, tree->root, 0);
        diag_info("=====");
    }
}

void values_delete(rbtree_t *tree, int *values, int len)
{
    diag_info("start delete node");
    for (int i = 0; i < len; i++) {
        rbtree_dump(tree, tree->root, 0);
        rbtree_delete(tree, (void *)&values[i]);
        diag_info("=====");
    }
}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
int main()
{
    struct rbtree_ops_s ops = {
        value_cmp,
        value_dup,
        value_dump
    };

    rbtree_t *tree = rbtree_create(&ops);

    int values[MAX_SIZE] = {0};
    int len =  ARRAY_SIZE(values);
    values_init(values, len);
    values_insert(tree, values, len);

    values_shuffle(values, len);
    values_delete(tree, values, len);
    return 0;
}