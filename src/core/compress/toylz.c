/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/

#include "toylz.h"

#include <stdint.h>

#include "pub_def.h"
#include "dict.h"

/**
 * 1、压缩文件格式：
 *    +========+=======+     +=======+
 *    | Header | block | ... | block |
 *    +========+=======+     +=======+
 * 
 * 2、block的分为token和block data两个部分，格式如下：
 *      +=======+============+
 *      | token | block data |
 *      +=======+============+
 *
 *    token分为literal和match两类。literal表示正常的字符串，match表示对于前向
 *    字符串的引用，存储（length, distance)两个。
 *  
 *    2.1 Literal
 *      M1M0 表示L1~Ln+5的占用字节,L1~Ln+5表示block data的长度。格式如下：
 *        +-----------------+-+ ... +-+============+
 *        | 0M1M0 Ln+5~Ln+1 |  Ln~L1  | block data |
 *        +-----------------+-+ ... +-+============+
 *
 *      具体的例子如下
 *        1、 Literal 长度在1b~31b的长度
 *          +-----------+============+
 *          | 000 L5-L1 | block data |
 *          +-----------+============+
 *        2、 Literal长度在32b~8k之间
 *          +------------+-------+============+
 *          | 001 L13-L9 | L8-L1 | block data |
 *          +------------+-------+============+
 *
 *    2.2 Match
 *      A1~A3表示L1~Ln占用的长度，B1~B3表示D1~Dn的占用的长度，L1~Ln表示长度，
 *      D1~Dn表示距离。block格式如下：
 *      +---------------+-+ ... +-+-+ ... +-+
 *      | 11A3~A1B3~B1 |  Ln~L1  |  Dn~D1  |
 *      +---------------+-+ ... +-+-+ ... +-+
 */

/**
 *  不同level下的滑窗大小：
 *   level  ~  sliding windows
 *     0    ~       4k
 *     1    ~       8k
 *     2    ~       16k
 *     3    ~       32k
 *     4    ~       64k
 *     5    ~       128k
 *     6    ~       256k
 *     7    ~       512k
 *     8    ~       1M
 *     9    ~       2M
 */
static int lz_calculate_block_size(int level)
{
    if (level > LZ_MAX_COMPRESS_LEVEL ||
        level < LZ_MIN_COMPRESS_LEVEL) {
      return TOY_ERR_LZ_LEVEL_INVALID;
    }

    uint8_t block_size_pow2[] = {
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21
    };

    option->block_size = ((uint32_t)1) << block_size_pow2[level];
    return TOY_OK;
}

lz_compressor_t *lz_create_compressor(lz_options_t *option,
    const uint8_t *in, uint32_t in_len)
{
    lz_compressor_t *compressor = calloc(1, sizeof(*compressor));
    if (!compressor) {
        return NULL;
    }

    compressor->in.stream = in;
    compressor->in.total_len = in_len;
    compressor->in.cur_pos = 0;

    compressor->block_size = lz_calculate_block_size(option->level);
    return compressor;
}

#define MIN_INPUT_LEN 4
#define SEQ_SIZE 4
#define INVALID_POS 0xffff

typedef struct {
    uint32_t seq;
    uint32_t pos;
} lz_refpos_t;

static uint32_t tz_hash_func(const void *key)
{
    lz_refpos_t *node = (lz_refpos_t *)key;
    return dict_int_hash_func(node->seq);
}

static bool tz_key_match(const void *key1, const void *key2)
{
    lz_refpos_t *node1 = (lz_refpos_t *)key1;
    lz_refpos_t *node2 = (lz_refpos_t *)key2;
    return node1->seq == node2->key2;
}

static uint32_t lz_read_seq(const void *ptr)
{
    return *(uint32_t *)ptr;
}

/* 创建lz字典 */
static dict_t *lz_create_dict()
{
    dict_config_t dict_config = {
        .priv_data = NULL,
        .hash_func = tz_hash_func,
        .key_match = tz_key_match
    };

    dict_t *dict = dict_create(&dict_config);
    if (dict != NULL) {
        return NULL;
    }

    return dict;
}

static lz_refpos_t *lz_get_refpos(dict_t *lz_dict, uint32_t seq)
{
    lz_refpos_t key = {
        .seq = seq, 
    };

    dict_entry_t *entry = dict_find(lz_dict, &key);
    if (entry == NULL) {
        return NULL;
    }

    return entry->record;
}

static int lz_update_refpos(dict_t *lz_dict, uint32_t seq, uint32_t refpos)
{
    lz_refpos_t key = {
        .seq = seq,
        .pos = refpos
    };

    dict_entry_t *entry = dict_find(lz_dict, &key);
    if (entry != NULL) {
        ((lz_refpos_t *)entry->record)->pos = refpos;
        return TOY_OK;
    }

    int ret = dict_add(lz_dict, &key);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;;
}

static void lz_destroy_dict(dict_t *lz_dict)
{
    dict_destroy(lz_dict);
}

static void lz_encode_literals(const uint8_t *in, uint32_t start, uint32_t end,
    uint8_t *out, uint32_t *out_pos, uint32_t out_len)
{
    if (out_len - *out_pos < end - start) {
        return TOY_ERR_LZ_OUT_MEM_UNSUFFICIENT;
    }
    (void)memcpy(out + *out_pos, in + start, end - out);
    *out_pos = *out_pos + (end - start);
}

static void lz_encode_match(const uint8_t *in, uint32_t ref_pos,
    uint32_t cur_pos, uint32_t in_len, uint8_t *out, uint32_t *out_pos, uint32_t out_len)
{

}

static int lz_compress_block(const uint8_t *in, uint32_t start, uint32_t end,
            uint8_t *out, uint32_t out_len, uint32_t *out_pos)
{
    /* 初始化哈希表 */
    dict_t *lz_dict = lz_create_dict();
    if (lz_dict == NULL) {
        return TOY_ERR_LZ_DICT_CREATE_FAIL;
    }

    uint32_t pos = start;
    uint32_t anchor = start;
    while (pos < end - SEQ_SIZE) {
        uint32_t seq = lz_read_seq(in + pos);
        lz_refpos_t *refpos = lz_get_refpos(lz_dict, seq);
        if (refpos != NULL) {
            lz_encode_literals(in, anchor, refpos->pos, out, out_pos, out_len);
            lz_encode_match();
            // anchor move
            pos = anchor;
        } else {
            pos = pos + 1;
        }

        lz_update_refpos(lz_dict, seq, pos);
    }
    
    lz_destroy_dict(lz_dict);
    return TOY_OK;
}

int lz_compress(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t out_len,
    lz_options_t *option);
{
    if (in_len < MIN_INPUT_LEN) {
        return TOY_ERR_LZ_INVALID_PARA;
    }

    uint32_t block_start = 0;
    uint32_t block_size = option->block_size;
    uint32_t out_pos = 0;
    while (block_start < in_len) {
        uint32_t block_end = block_start + block_size < in_len ?
            block_start + block_size : in_len;

        int ret = lz_compress_block(in, block_start, block_end, out, out_len, &out_pos);
        if (ret != TOY_OK) {
          return ret;
        }

        block_start = block_end;
    }
    return TOY_OK;
}

