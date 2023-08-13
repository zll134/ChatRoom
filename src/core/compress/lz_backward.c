/* ********************************
 * Author:       Zhanglele
 * Description:  压缩后向引用管理
 * create time:  2023.07.30
 ********************************/

#include "lz_backward.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pub_def.h"
#include "hash.h"
#include "dict.h"

static uint32_t lz_backward_hash_func(const void *key)
{
    lz_backward_t *node = (lz_backward_t *)key;
    return dict_int_hash_func(node->seq);
}

static bool lz_backward_key_match(const void *key1, const void *key2)
{
    lz_backward_t *node1 = (lz_backward_t *)key1;
    lz_backward_t *node2 = (lz_backward_t *)key2;
    return node1->seq == node2->seq;
}

/* 创建后向引用字典 */
dict_t *lz_create_backward_dict()
{
    dict_config_t dict_config = {
        .priv_data = NULL,
        .hash_func = lz_backward_hash_func,
        .key_match = lz_backward_key_match
    };

    dict_t *dict = dict_create(&dict_config);
    if (dict == NULL) {
        return NULL;
    }

    return dict;
}

lz_backward_t *lz_get_backward(dict_t *dict, uint32_t seq)
{
    lz_backward_t key = {
        .seq = seq, 
    };

    dict_entry_t *entry = dict_find(dict, &key);
    if (entry == NULL) {
        return NULL;
    }

    return entry->record;
}

int lz_createorset_backward(dict_t *dict, uint32_t seq, uint32_t refpos)
{
    lz_backward_t key = {
        .seq = seq,
        .refpos = refpos
    };

    dict_entry_t *entry = dict_find(dict, &key);
    if (entry != NULL) {
        ((lz_backward_t *)entry->record)->refpos = refpos;
        return TOY_OK;
    }

    int ret = dict_add(dict, &key, sizeof(key));
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

void lz_destroy_backward_dict(dict_t *dict)
{
    dict_destroy(dict);
}