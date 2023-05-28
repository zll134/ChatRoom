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
    return (dict->rehash_idx != 0);
}

dict_t *dict_create(dict_config_t *config)
{
    dict_t *dict = calloc(1, sizeof(dict_t));
    if (!dict) {
        diag_err("[dict] Create dict failed");
        return NULL;
    }

    (void)memcpy(&dict->config, config, sizeof(*config));

    dict->rehash_idx = -1;
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
        return TOY_ERR_INVALID_REHARSH;
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
        return 
    }

    /* 如果rehash正在进行，则不能进行重置大小 */
    dict->ht[1] = ht;
    dict->rehash_idx = 0;
    return TOY_OK;
}

int dict_add(dict_t *dict, void *key, void *value)
{
    return TOY_OK;
}

int dict_set(dict_t *dict, void *key, void *value)
{
    return TOY_OK;
}

/* 删除哈希表中的键 */
int dict_delete(dict_t *dict, void *key)
{
    return TOY_OK;
}