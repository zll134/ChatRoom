/* ********************************
 * Author:       Zhanglele
 * Description:  hash表管理模块
 * create time:  2023.05.21
 ********************************/
#ifndef DICT_H
#define DICT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct tag_dict_entry {
    void *record;
    uint32_t record_size;
    struct tag_dict_entry *next;
} dict_entry_t;

typedef struct {
    dict_entry_t **table;
    uint32_t size;
    uint32_t sizemask;
    uint32_t used;
} dict_htable_t;

typedef struct {
    uint64_t total_search_len;
    uint64_t oper_times;  /* 包括查询、添加、删除的操作的次数 */
} dict_stat_t;

typedef struct {
    void *priv_data;
    uint32_t (*hash_func)(const void *record);
    bool (*key_match)(const void *key1, const void *key2);
} dict_config_t;

typedef struct {
    dict_htable_t ht[2];  /* 设置两个哈希表用于渐进式rehash */
    dict_config_t config; /* 哈希表配置 */
    int rehash_idx;       /* 记录正在rehash的哈希表编号, -1表示未重哈希 */
    dict_stat_t stat;
} dict_t;

/* 创建哈希表对象 */
dict_t *dict_create(dict_config_t *config);

/* 释放哈希表对象 */
void dict_destroy(dict_t *dict);

/* 改变哈希表的长度。
 * @warning: 如果正在进行rehash，该操作会失败
 **/
int dict_resize(dict_t *dict, uint32_t size);

/* 向哈希表中添加键值对, record中存储键值对 */
int dict_add(dict_t *dict, void *record, uint32_t record_size);

/* 删除哈希表中的键， record中填写key值即可 */
int dict_delete(dict_t *dict, void *record);

/**
 * 获取哈希表中节点的数量
 */
uint32_t dict_get_entry_num(dict_t *dict);

/**
 * 根据key值查找哈希表中的对应节点
 */
dict_entry_t *dict_find(dict_t *dict, const void *record);

/**
 * 获取拉链法中平均节点搜索长度
 */
int dict_get_average_search_len(dict_t *dict);

#endif