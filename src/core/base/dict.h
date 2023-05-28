/* ********************************
 * Author:       Zhanglele
 * Description:  hash表管理模块
 * create time:  2023.05.21
 ********************************/
#ifndef DICT_H
#define DICT_H

#include <stdint.h>

typedef struct dictEntry {
    void *key;
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next;
} dictEntry;

typedef struct {
    dictEntry **table;
    uint32_t size;
    uint32_t sizemask;
    uint32_t used;
} dict_htable_t;

typedef struct {
    dict_htable_t ht[2];
    dict_config_t config;
} dict_t;

typedef struct {
    void *priv_data;
    uint32_t (*hash_func)(const void *key);
} dict_config_t;

/* 创建哈希表对象 */
dict_t *dict_create(dict_config_t *config);

/* 释放哈希表对象 */
void dict_destroy(dict_t *dict);

int dict_add(dict_t *dict, void *key, void *value);
int dict_delete(dict_t *dict, void *key);

#endif