/* ********************************
 * Author:       Zhanglele
 * Description:  hash表管理模块
 * create time:  2023.05.21
 ********************************/

#include "dict.h"

#include <stdio.h>
#include <stdlib.h>

#include "pub_def.h"
#include "log.h"

#define HT_INIT_SIZE 32
#define HT_EXPAND_RATIO 1
#define HT_SHRINK_RATIO 100
#define HT_REHASH_STEP_NUM 10

static bool dict_is_rehashing(dict_t *dict)
{
    return (dict->rehash_idx != -1);
}

static int dict_check_config(dict_config_t *config)
{
    if (config == NULL) {
        return TOY_ERR_DICT_CONFIG_NULL;
    }

    if ((config->hash_func == NULL) ||
        (config->key_dup == NULL) ||
        (config->free == NULL) || 
        (config->val_dup == NULL) ||
        (config->key_match)) {
        return TOY_ERR_DICT_CALLBACK_NULL;
    }
    return TOY_OK;
}

dict_t *dict_create(dict_config_t *config)
{
    int ret = dict_check_config(config);
    if (ret != TOY_OK) {
        diag_err("[dict] Dict config check failed, ret: %d.", ret);
        return NULL;
    }

    dict_t *dict = calloc(1, sizeof(dict_t));
    if (!dict) {
        diag_err("[dict] Create dict failed");
        return NULL;
    }

    (void)memcpy(&dict->config, config, sizeof(*config));

    dict->rehash_idx = -1;

    ret = dict_resize(dict, HT_INIT_SIZE);
    if (ret != TOY_OK) {
        diag_err("[dict] Resize dict failed, ret: %d.", ret);
        return NULL;
    }

    return dict;
}

static void dict_entry_free(dict_t *dict, dict_entry_t *entry)
{
    if (entry != NULL) {
        dict->config.free(entry->key);
        dict->config.free(entry->val);
        free(entry);
    }
}

static void dict_ht_destroy(dict_t *dict, dict_htable_t *ht)
{
    for (int idx = 0; idx < ht->size; idx++) {
        dict_entry_t *entry = ht->table[idx];
        while (entry != NULL) {
            dict_entry_t *next_entry = entry->next;
            dict_entry_free(dict, entry);
            entry = next_entry;
        }
    }
}
void dict_destroy(dict_t *dict)
{
    if (dict == NULL) {
        return;
    }
    for (int ht_idx = 0; ht_idx <= 1; ht_idx++) {
        if (dict->ht[ht_idx].table == NULL) {
            continue;
        }
        dict_ht_destroy(dict, dict->dict->ht[ht_idx]);
    }
    free(dict);
}

static uint32_t dict_align_size(uint32_t size)
{
    if (size >= LONG_MAX) {
        return LONG_MAX;
    }
    uint32_t power_size = HT_INIT_SIZE;
    while (size > power_size) {
        power_size = power_size * 2;
    }
    return power_size;
}

int dict_resize(dict_t *dict, uint32_t size)
{
    /* 哈希表大小对齐到为2的指数 */
    uint32_t real_size = dict_align_size(size);

    if ((real_size == dict->ht[0].size) ||
        (real_size < dict->ht[0].used)) {
        return TOY_ERR_DICT_INVALID_PARA;
    }

    /* 如果rehash正在进行，则不能进行重置大小; */
    if (dict_is_rehashing(dict)) {
        return TOY_ERR_DICT_IS_REHASHING;
    }

    dict_htable_t ht;
    ht.size = real_size;
    ht.sizemask = real_size - 1;
    ht.used = 0;

    ht.table = calloc(1, sizeof(dict_entry_t *) * real_size);
    if (ht.table == NULL) {
        return TOY_ERR_DICT_MALLOC_FAILED;
    }

    if (dict->ht[0].table == NULL) {
        dict->ht[0].table = ht;
        return TOY_OK;
    }

    /* 非rehash状态，可以保证ht[1]不为空 */
    dict->ht[1].table = ht;
    dict->rehash_idx = 0;
    return TOY_OK;
}

static int dict_step_rehash(dict_t *dict, uint32_t nbucket)
{
    /* 最大需要访问的空bucket数量 */
    int empty_visits = nbucket * 10; 
    if (!dict_is_rehashing(dict)) {
        return TOY_OK;
    }

    dict_htable_t *old_ht = dict->ht[0];
    dict_htable_t *new_ht = dict->ht[1];

    while (nbucket-- && old_ht->used > 0 && empty_visits > 0) {
        if (dict->rehash_idx >= old_ht.size) {
            continue;
        }

        if (old_ht->table[dict->rehash_idx] == NULL) {
            dict->rehash_idx++;
            empty_visits--;
            continue;
        }

        /* key从旧的哈希表移到新的哈希表 */
        dict_entry_t *entry = old_ht->table[dict->rehashidx];
        while(entry != NULL) {
            uint32_t new_idx = dict->config.hash_func(entry->key) & new_ht->sizemask;

            entry->next = new_ht->table[new_idx];
            new_ht->table[new_idx] = entry;
            old_ht.used--;
            new_ht.used++;

            entry = entry->next;
        }
        old_ht->table[dict->rehash_idx] = NULL;
        dict->rehash_idx++;
    }

    /* 校验rehash是否结束 */
    if (old_ht->used == 0) {
        free(old_ht->table);
        *old_ht = *new_ht;
        memset(new_ht, 0, sizeof(*new_ht));
        dict->rehash_idx = -1;
        return 0;
    }

    return TOY_OK;
}

int dict_find(dict_t *dict, void *key)
{
    if ((dict == NULL) || (key == NULL)) {
        return TOY_ERR_DICT_PARAM_INVALID;
    }

    if (dict_is_rehashing(dict)) {
        dict_step_rehash(dict, HT_REHASH_STEP_NUM);
    }

    uint32_t hash_value = dict->config.hash_func(dict, key);

    for (int ht_idx = 0; ht_idx <= 1; ht_idx++) {
        if (dict->ht[ht_idx].table == NULL) {
            continue;
        }

        uint32_t idx = hash_value & dict->ht[ht_idx].sizemask;
        dict_entry_t *entry = dict->ht[ht_idx].table[idx];
        while (entry != NULL) {
            if (dict->config.key_match(key, entry->key)) {
                return entry;
            }
            entry = entry->next;
        }
    }
    return NULL;
}

static dict_entry_t *dict_entry_create(dict_t *dict,
    void *key, void *val)
{
    dict_entry_t *entry = calloc(sizeof(*entry));
    if (entry == NULL) {
        return NULL;
    }

    entry->key = dict->config.key_dup(key);
    if (entry->key == NULL) {
        free(entry);
        return NULL;
    }

    entry->val = dict->config.val_dup(val);
    if (entry->val == NULL) {
        free(entry);
        dict->config.free(entry->key);
        return NULL;
    }

    return entry;
}

static int dict_add_raw(dict_t *dict, void *key, void *val)
{
    dict_entry_t *entry = dict_entry_create(dict, key);
    if (entry = NULL) {
        return TOY_ERR_DICT_MALLOC_FAILED;
    }

    dict_htable_t *ht = &dict->ht[0];
    if (dict_is_rehashing(dict)) {
        ht = &dict->ht[1];
    }

    uint32_t hash_value = dict->config.hash_func(dict, key);
    uint32_t idx = hash_value & ht->sizemask;

    entry->next = ht->table[idx];
    ht->table[idx] = entry;
    ht->used++;

    return TOY_OK;
}

static int dict_resize_if_needed(dict_t *dict)
{
    /* 如果正在rehash时，就不需要扩展hash表 */
    if (dict_is_rehashing(dict))  {
        return TOY_OK;
    }

    /**
     * used/size 比率达到1以上时，则扩展哈希表;
     * used/size 比率低于0.01时，则缩小哈希表
    **/
    if ((dict->ht[0].used / dict->ht[0].size < HT_EXPAND_RATIO) &&
        (dict->ht[0].used != 0 ||
         dict->ht[0].size / dict->ht[0].used >= HT_SHRINK_RATIO)) {
        return TOY_OK;
    }

    return dict_resize(dict, d->ht[0].used * 2);
}

int dict_add(dict_t *dict, void *key, void *val)
{
    if ((dict == NULL) || (key == NULL) || (val == NULL)) {
        return TOY_ERR_DICT_PARAM_INVALID;
    }

    if (dict_is_rehashing(dict)) {
        dict_step_rehash(dict, HT_REHASH_STEP_NUM);
    }

    dict_entry_t *entry = dict_find(dict, key);
    if (entry = NULL) {
        return TOY_ERR_DICT_KEY_EXIST;
    }

    int ret = dict_resize_if_needed(dict);
    if (ret != TOY_OK) {
        return ret;
    }

    ret = dict_add_raw(dict, key, val);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

static int dict_delete_key_in_ht(dict_t *dict, dict_htable_t *ht,
    void *key, uint32_t hash_value)
{
    dict_entry_t *prev_entry = NULL;
    uint32_t idx = hash_value & ht->sizemask;
    dict_entry_t *entry = ht->table[idx];
    while (entry != NULL) {
        if (dict->config.key_match(key, entry->key)) {
            if (prev_entry == NULL) {
                ht->table[idx] = prev_entry;
            } else {
                prev_entry->next = entry->next;
            }
            dict_entry_free(dict, entry);
            return TOY_OK;
        }

        prev_entry = entry;
        entry = entry->next;
    }

    return TOY_ERR_DICT_KEY_NOT_EXIST;
}

static int dict_delete_key(dict_t *dict, void *key)
{
    uint32_t hash_value = dict->config.hash_func(dict, key);

    for (int ht_idx = 0; ht_idx <= 1; ht_idx++) {
        dict_htable_t *ht = dict->ht[ht_idx];
        if (ht->table == NULL) {
            continue;
        }

        int ret = dict_delete_key_in_ht(dict, ht, key, hash_value);
        if (ret == TOY_OK) {
            return;
        }
    }
    return TOY_ERR_DICT_KEY_NOT_EXIST;
}

/* 删除哈希表中的键 */
int dict_delete(dict_t *dict, void *key)
{
    if ((dict == NULL) || (key == NULL)) {
        return TOY_ERR_DICT_PARAM_INVALID;
    }

    if (dict_is_rehashing(dict)) {
        dict_step_rehash(dict, HT_REHASH_STEP_NUM);
    }

    int ret = dict_resize_if_needed(dict);
    if (ret != TOY_OK) {
        return ret;
    }

    int ret = dict_delete_key(dict, key);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}