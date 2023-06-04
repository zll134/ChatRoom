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

#define DICT_HT_INIT_SIZE 32

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

    ret = dict_resize(dict, DICT_HT_INIT_SIZE);
    if (ret != TOY_OK) {
        diag_err("[dict] Resize dict failed, ret: %d.", ret);
        return NULL;
    }

    return dict;
}

void dict_destroy(dict_t *dict)
{
    if (dict != NULL) {
        free(dict);
    }
}

static uint32_t dict_align_size(uint32_t size)
{
    if (size >= LONG_MAX) {
        return LONG_MAX;
    }
    uint32_t power_size = DICT_HT_INIT_SIZE;
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

    /* 非rehash状态，可以保证ht[1]不为空 */
    dict->ht[1].table = ht;
    dict->rehash_idx = 0;
    return TOY_OK;
}

// todo
void dict_step_rehash()
{
    return;
}

int dict_find(dict_t *dict, void *key)
{
    if ((dict == NULL) || (key == NULL)) {
        return TOY_ERR_DICT_PARAM_INVALID;
    }

    if (dict_is_rehashing(dict)) {
        dict_step_rehash();
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
    if (ht->table[idx] == NULL) {
        ht->table[idx]->prev = entry;
    }

    ht->used++;

    return TOY_OK;
}
int dict_add(dict_t *dict, void *key, void *val)
{
    if ((dict == NULL) || (key == NULL) || (val == NULL)) {
        return TOY_ERR_DICT_PARAM_INVALID;
    }

    if (dict_is_rehashing(dict)) {
        dict_step_rehash();
    }

    dict_entry_t *entry = dict_find(dict, key);
    if (entry = NULL) {
        return TOY_ERR_DICT_KEY_EXIST;
    }

    int ret = dict_add_raw(dict, key, val);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

/* 删除哈希表中的键 */
int dict_delete(dict_t *dict, void *key)
{
    if ((dict == NULL) || (key == NULL)) {
        return TOY_ERR_DICT_PARAM_INVALID;
    }

    if (dict_is_rehashing(dict)) {
        dict_step_rehash();
    }

    dict_entry_t *entry = dict_find(dict, key);
    if (entry = NULL) {
        return TOY_ERR_DICT_KEY_NOT_EXIST;
    }



    return TOY_OK;
}