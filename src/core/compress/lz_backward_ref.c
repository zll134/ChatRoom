/* ********************************
 * Author:       Zhanglele
 * Description:  压缩后向引用管理
 * create time:  2023.07.30
 ********************************/

#include "lz_backward_ref.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pub_def.h"
#include "log.h"
#include "hash.h"
#include "dict.h"
#include "ringbuff.h"

typedef struct {
    uint32_t seq;    // 四字节序列
    ringbuff_t *refs; // 所有引用位置
} lz_backward_ref_t;

#define MAX_REF_POS_NUM 16

static uint32_t lz_backward_hash_func(const void *key)
{
    lz_backward_ref_t *node = (lz_backward_ref_t *)key;
    return dict_int_hash_func(node->seq);
}

static bool lz_backward_key_match(const void *key1, const void *key2)
{
    lz_backward_ref_t *node1 = (lz_backward_ref_t *)key1;
    lz_backward_ref_t *node2 = (lz_backward_ref_t *)key2;
    return node1->seq == node2->seq;
}

static void lz_backward_entry_uninit(void *record)
{
    lz_backward_ref_t *node = (lz_backward_ref_t *)record;
    ringbuff_destroy(node->refs);
    node->refs = NULL;
}

static int lz_backward_entry_init(void *record)
{
    lz_backward_ref_t *node = (lz_backward_ref_t *)record;

    ringbuff_option_t option = {0};
    option.buff_size = sizeof(uint32_t) * MAX_REF_POS_NUM;

    node->refs = ringbuff_create(&option);
    if (node->refs == NULL) {
        return TOY_ERR_LZ_RINGBUF_CREATE_FAIL;
    }

    return TOY_OK;
}

/* 创建后向引用字典 */
dict_t *lz_create_backward_ref_dict()
{
    dict_config_t dict_config = {
        .priv_data = NULL,
        .hash_func = lz_backward_hash_func,
        .key_match = lz_backward_key_match,
        .record_init = lz_backward_entry_init,
        .record_uninit = lz_backward_entry_uninit
    };

    dict_t *dict = dict_create(&dict_config);
    if (dict == NULL) {
        return NULL;
    }

    return dict;
}

uint32_t lz_get_backward_refs(dict_t *dict, uint32_t seq, uint32_t **refposes)
{
    if (dict == NULL || refposes == NULL) {
        return 0;
    }

    lz_backward_ref_t key = {
        .seq = seq, 
    };

    dict_entry_t *entry = dict_find(dict, &key);
    if (entry == NULL) {
        return 0;
    }

    ringbuff_t *refs_buf = ((lz_backward_ref_t *)entry->record)->refs;
    uint32_t refs_num = ringbuff_len(refs_buf) / sizeof(uint32_t);
    if (refs_num == 0) {
        return 0;
    }

    uint32_t refs_size = refs_num * sizeof(uint32_t);
    uint32_t *refs = malloc(refs_size);
    if (refs == NULL) {
        return 0;
    }

    int ret = ringbuff_read(refs_buf, (uint8_t *)refs, refs_size);
    if (ret == -1) {
        free(refposes);
        return 0;
    }

    *refposes = refs;
    return refs_num;
}

static int lz_add_backward_refpos(lz_backward_ref_t *backward_ref, uint32_t refpos)
{
    return ringbuff_write(backward_ref->refs, (uint8_t *)&refpos, sizeof(refpos));
}

int lz_insert_backward_ref(dict_t *dict, uint32_t seq, uint32_t refpos)
{
    lz_backward_ref_t key = {
        .seq = seq,
    };

    dict_entry_t *entry = dict_find(dict, &key);
    if (entry != NULL) {
        // key存在时, 则已有存在refs中添加新的引用位置
        return lz_add_backward_refpos(entry->record, refpos);
    }

    // key不存在时, 则添加新的key
    int ret = dict_add(dict, &key, sizeof(key));
    if (ret != TOY_OK) {
        return ret;
    }

    entry = dict_find(dict, &key);
    return lz_add_backward_refpos(entry->record, refpos);
}

void lz_destroy_backward_ref_dict(dict_t *dict)
{
    dict_destroy(dict);
}