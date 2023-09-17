/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/

#include "toylz.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pub_def.h"
#include "log.h"
#include "dict.h"
#include "bit_op.h"
#include "lz_backward_ref.h"

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
 *      M1M0 表示L1~Ln的占用字节,L1~Ln+5表示block data的长度。格式如下：
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
 *      格式1:
 *          +---------------+-+ ... +-+-+ ... +-+
 *          | 10A4~A1B2~B1  |  Ln~L1  |  Dn~D1  |
 *          +---------------+-+ ... +-+-+ ... +-+
 *         A1~A4表示L1~Ln占用的长度，B1~B2表示D1~Dn的占用的长度，L1~Ln表示长度，
 *         D1~Dn表示距离。
 *      格式2:
 *          +---------------+-+ ... +-+
 *          | 11L4~L1B2~B1  |  Dn~D1  |
 *          +---------------+-+ ... +-+
 *         L1~L4表示匹配串的长度，B1~B2表示D1~Dn的占用的长度，D1~Dn表示距离。
 */


#define MIN_INPUT_LEN 4
#define SEQ_SIZE 4
#define INVALID_POS 0xffff
#define MATCH_LEN_BITS 4
#define DISTANCE_BITS 2
#define SMALL_MATCH_LEN_MAX  ((uint32_t)1 << MATCH_LEN_BITS - 1) + SEQ_SIZE

enum {
    MATCH_TOKEN_LEN_BYTE_ENCODE,
    MATCH_TOKEN_LEN_ENCODE
};

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
static int lz_calculate_sliding_win(int level)
{
    uint8_t block_size_pow2[] = {
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21
    };

    return ((uint32_t)1) << block_size_pow2[level];
}

lz_compressor_t *lz_create_compressor(lz_option_t *option)
{
    if (option->level > LZ_MAX_COMPRESS_LEVEL ||
        option->level < LZ_MIN_COMPRESS_LEVEL) {
        diag_err("[compress] Compress level is invalid.");
        return NULL;
    }

    lz_compressor_t *comp = calloc(1, sizeof(*comp));
    if (!comp) {
        diag_err("[compress] Calloc for compressor failed.");
        return NULL;
    }

    comp->sliding_win = lz_calculate_sliding_win(option->level);
    return comp;
}

void lz_destroy_compressor(lz_compressor_t *comp)
{
    if (comp != NULL && comp->backward_refs != NULL) {
        lz_destroy_backward_ref_dict(comp->backward_refs);
    }

    if (comp != NULL) {
        free(comp);
    }
}

static uint32_t lz_read_seq(const void *ptr)
{
    return *(uint32_t *)ptr;
}

static void lz_encode_literals_header(lz_stream_t *strm,
    uint32_t literal_len)
{
    uint8_t *out = strm->out;
    if (literal_len <= 31) {
        out[strm->out_pos] = literal_len;
        strm->out_pos += 1;
        return;
    } else if (literal_len <= (((uint32_t)1) << 13 - 1)) {
        out[strm->out_pos + 1] = (literal_len & 0xff);
        out[strm->out_pos] = (literal_len >> 8) & 0xff;
        out[strm->out_pos] |= 0x20;
        strm->out_pos += 2;
        return;
    } else if (literal_len <= (((uint32_t)1) << 21 - 1)) {
        out[strm->out_pos + 2] = (literal_len & 0xff);
        out[strm->out_pos + 1] = ((literal_len >> 8) & 0xff);
        out[strm->out_pos] = (literal_len >> 16) & 0xff;
        out[strm->out_pos] |= 0x40;
        return;
    }
}

static void lz_encode_literals(lz_stream_t *strm, uint32_t start, uint32_t end)
{
    if (strm->out_size - strm->out_pos < end - start) {
        return;
    }

    lz_encode_literals_header(strm, end - start);

    (void)memcpy(strm->out + strm->out_pos, strm->in + start, end - start);
    strm->out_pos += (end - start);
}

static uint32_t lz_get_match_len(const uint8_t *in, uint32_t in_len,
    uint32_t ref_pos, uint32_t cur_pos)
{
    uint32_t len = 0;
    while (ref_pos + len < cur_pos && cur_pos + len < in_len) {
        if (in[ref_pos + len] == in[cur_pos + len]) {
            len++;
            continue;
        }
        break;
    }
    return len;
}

static uint32_t lz_encode_match(lz_stream_t *strm, uint32_t ref_pos)
{
    uint32_t match_len = lz_get_match_len(strm->in, strm->in_size, ref_pos, strm->in_pos);
    uint32_t distance = strm->in_pos - ref_pos;
    uint8_t *out = strm->out;

    uint8_t len_bytes = bit_get_bytes(match_len);
    uint8_t dist_bytes = bit_get_bytes(distance);
    uint8_t skip_len = 0;
    out[strm->out_pos] = 0;
    if (match_len > SMALL_MATCH_LEN_MAX) {
        out[strm->out_pos] |= 0x80; // 1000 0000
        out[strm->out_pos] |= (len_bytes << DISTANCE_BITS);
        out[strm->out_pos] |= dist_bytes;
        skip_len = dist_bytes + len_bytes;
    } else {
        out[strm->out_pos] |= 0xc0; // 1100 0000
        out[strm->out_pos] |= ((match_len - SEQ_SIZE) << DISTANCE_BITS);
        out[strm->out_pos] |= dist_bytes;
        skip_len = dist_bytes;
        len_bytes = 0;
    }

    // 长度编码
    uint8_t tmp_len_bytes = len_bytes;
    uint32_t tmp_match_len = match_len;
    while ((match_len > SMALL_MATCH_LEN_MAX) &&
           (tmp_len_bytes > 0)) {
        out[strm->out_pos + tmp_len_bytes] = tmp_match_len & 0xff;
        tmp_len_bytes--;
        tmp_match_len = tmp_match_len >> 8;
    }

    // 距离编码
    uint8_t tmp_dist_bytes = dist_bytes;
    while (tmp_dist_bytes > 0) {
        out[strm->out_pos + len_bytes + tmp_dist_bytes] = distance & 0xff;
        tmp_dist_bytes--;
        distance = distance >> 8;
    }

    strm->out_pos += (skip_len + 1);

    return match_len;
}

static int lz_get_longest_match(lz_stream_t *strm, uint32_t *refs, uint32_t refs_num)
{
    uint32_t max_len = 0;

    uint32_t target_ref_pos = refs[0];
    for (int i = 0; i < refs_num; i++) {
        uint32_t match_len = lz_get_match_len(strm->in, strm->in_size, refs[i], strm->in_pos);
        if (match_len >= max_len) {
            max_len = match_len;
            target_ref_pos = refs[i];
        }
    }

    return target_ref_pos;
}

static int lz_skip_match(lz_compressor_t *comp, lz_stream_t *strm, uint32_t match_len)
{
    for (int i = 0; i < match_len; i++) {
        if (strm->in_pos >= strm->in_size - SEQ_SIZE) {
            continue;
        }

        uint32_t seq = lz_read_seq(strm->in + strm->in_pos);
        int ret = lz_insert_backward_ref(comp->backward_refs, seq, strm->in_pos);
        if (ret != TOY_OK) {
            diag_err("[compress] Insert ref failed, in_pos %u.", strm->in_pos);
            return ret;
        }
        
        strm->in_pos++;
    }
    return TOY_OK;
}

static int lz_encode_stream(lz_compressor_t *comp, lz_stream_t *strm, uint32_t *anchor)
{
    uint32_t seq = lz_read_seq(strm->in + strm->in_pos);

    /* 向后查找具有相同4字节前缀的所有字符串位置 */
    uint32_t *refs;
    uint32_t refs_num = lz_get_backward_refs(comp->backward_refs, seq, &refs);
    if (refs_num == 0) {
        lz_insert_backward_ref(comp->backward_refs, seq, strm->in_pos);
        strm->in_pos++;
        return TOY_ERR_LZ_BACKWARD_NOT_EXIST;
    }

    /* 压缩literal */
    lz_encode_literals(strm, *anchor, strm->in_pos);

    /* 压缩matches */
    uint32_t longest_ref = lz_get_longest_match(strm, refs, refs_num);
    uint32_t match_len = lz_encode_match(strm, longest_ref);

    // 跳过匹配段
    *anchor = strm->in_pos + match_len;

    return lz_skip_match(comp, strm, match_len);
}

/**
 *  函数中各个变量的含义：
 * 
 *  偏移:  0          anchor  refpos   in_pos                    in_size - 1
 *  数据:  |-------------|------|--------|-------------|---------------|
 *  段名:  |---written---|----literal----|----match----|--need handle--|
 */
static int lz_start_compress(lz_compressor_t *comp, lz_stream_t *strm)
{
    uint32_t anchor = 0;

    /* 压缩literal + match的数据 */
    while (strm->in_pos < strm->in_size - SEQ_SIZE) {
        int ret = lz_encode_stream(comp, strm, &anchor);
        if (ret == TOY_ERR_LZ_BACKWARD_NOT_EXIST) {
            continue;
        }
        if (ret != TOY_OK) {
            return ret;
        }
    }

    /* 压缩最后一段未处理的literal */
    if (anchor < strm->out_size) {
        lz_encode_literals(strm, anchor, strm->out_size);
    }
    return TOY_OK;
}

static void lz_init_strm(lz_stream_t *strm)
{
    strm->in_pos = 0;
    strm->out_pos = 0;
    strm->out_total = 0;
}

int lz_compress(lz_compressor_t *comp, lz_stream_t *strm)
{
    if (strm->in_size < MIN_INPUT_LEN) {
        return TOY_ERR_LZ_INVALID_PARA;
    }

    comp->backward_refs = lz_create_backward_ref_dict();
    if (comp->backward_refs == NULL) {
        diag_err("[compress] Create backward dict failed.");
        return TOY_ERR_LZ_CREATE_DICT_FAIL;
    }

    // 初始化字节流
    lz_init_strm(strm);

    // 开始压缩
    int ret = lz_start_compress(comp, strm);
    if (ret != TOY_OK) {
        diag_err("[compress] Compress failed, ret: %d.", ret);
        lz_destroy_backward_ref_dict(comp->backward_refs);
        comp->backward_refs = NULL;
        return ret;
    }

    strm->out_total = strm->out_pos;
    lz_destroy_backward_ref_dict(comp->backward_refs);
    comp->backward_refs = NULL;
    return TOY_OK;
}

static bool lz_is_literals_token(lz_stream_t *strm)
{
    uint8_t token = strm->in[strm->in_pos];

    if ((token & 0x80) > 0) { // 1000 0000
        return false;
    }
    return true;
}

static int lz_decode_literals(lz_stream_t *strm)
{
    uint32_t literal_len = 0;
    uint8_t token = strm->in[strm->in_pos];

    literal_len = token & 0x1f;
    uint8_t literal_len_byte = (token >> 5) & 0x3;
    strm->in_pos++;
    while (literal_len_byte > 0) {
        literal_len = (literal_len << 8) + strm->in[strm->in_pos];
        literal_len_byte--;
        strm->in_pos++;
    }
    memcpy(strm->out + strm->out_pos, strm->in + strm->in_pos, literal_len);
    strm->in_pos += literal_len;
    strm->out_pos += literal_len;

    return 0;
}

static int lz_decode_match(lz_stream_t *strm)
{
    uint8_t token = strm->in[strm->in_pos];
    uint8_t dist_bytes = token & 0x3; // 0000 0011

    uint32_t len = 0;
    uint32_t type;
    uint8_t len_bytes;
    if ((token & 0x40) > 0) { // 0100 0000
        type = MATCH_TOKEN_LEN_ENCODE;
        len = ((token >> DISTANCE_BITS) & 0xF) + SEQ_SIZE; // 0000 1111
    } else {
        type = MATCH_TOKEN_LEN_BYTE_ENCODE;
        len_bytes = (token >> DISTANCE_BITS) & 0xF; // 0000 1111
    }

    strm->in_pos++;
    while ((type == MATCH_TOKEN_LEN_BYTE_ENCODE) &&
           (len_bytes > 0)) {
        len = (len << 8) + (uint8_t)strm->in[strm->in_pos];
        strm->in_pos++;
        len_bytes--;
    }

    uint32_t dist = 0;
    while (dist_bytes > 0) {
        dist = (dist << 8) + (uint8_t)strm->in[strm->in_pos];
        strm->in_pos++;
        dist_bytes--;
    }

    memcpy(strm->out + strm->out_pos, strm->out + strm->out_pos - dist, len);
    strm->out_pos += len;
    return 0;
}

static int lz_decode_stream(lz_stream_t *strm)
{
    if (lz_is_literals_token(strm)) {
        return lz_decode_literals(strm);
    }

    return lz_decode_match(strm);
}

int lz_decompress(lz_compressor_t *comp, lz_stream_t *strm)
{
    if (strm->in_size < MIN_INPUT_LEN) {
        return TOY_ERR_LZ_INVALID_PARA;
    }

    lz_init_strm(strm);

    while (strm->in_pos < strm->in_size) {
        int ret = lz_decode_stream(strm);
        if (ret != TOY_OK) {
            return ret;
        }
    }
    strm->out_total = strm->out_pos;
    return TOY_OK;
}