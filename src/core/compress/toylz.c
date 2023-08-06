/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/

#include "toylz.h"

#include <stdint.h>

#include "pub_def.h"
#include "dict.h"
#include "bit_op.h"
#include "lz_backward.h"

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


#define MIN_INPUT_LEN 4
#define SEQ_SIZE 4
#define INVALID_POS 0xffff

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
      return TOY_ERR_LZ_LEVEL_INVALID;
    }

    lz_compressor_t *comp = calloc(1, sizeof(*comp));
    if (!comp) {
        return NULL;
    }

    comp->backward_dict = lz_create_backward_dict();
    if (comp->backward_dict == NULL) {
        return NULL;
    }

    comp->sliding_win = lz_calculate_sliding_win(option->level);
    return comp;
}

void lz_destroy_compressor(lz_compressor_t *comp)
{
    if (comp->backward_dict != NULL) {
        lz_destroy_backward_dict(comp->backward_dict);
    }

    if (!comp) {
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
    uint32_t 
    if (strm->out_len - strm->out_pos < end - start) {
        return TOY_ERR_LZ_OUT_MEM_UNSUFFICIENT;
    }
    lz_encode_literals_header(strm->out, strm->out_pos, end - start);

    (void)memcpy(strm->out + strm->out_pos, strm->in + start, end - start);
    strm->out_pos += (end - start);
}

static uint32_t lz_get_match_len(const uint8_t *in, uint32_t in_len,
    uint32_t ref_pos, uint32_t cur_pos)
{
    uint32_t off = 0;
    while (ref_pos + off < cur_pos && cur_pos + off < in_len) {
        if (in[ref_pos + off] == in[cur_pos + off]) {
            off++;
            continue;
        }
        break;
    }
    return off;
}

static uint32_t lz_encode_match(lz_stream_t *strm, uint32_t ref_pos)
{
    uint32_t match_len = lz_get_match_len(strm->in, strm->in_len, ref_pos, strm->in_pos);
    uint32_t tmp_match_len = match_len;
    uint32_t distance = strm->in_pos - ref_pos;

    uint8_t *out = strm->out;
    out[strm->out_pos] = 0;
    out[strm->out_pos] |= 0xc0;
    
    uint8_t match_len_bytes = bit_get_bytes(match_len);
    uint8_t distance_bytes = bit_get_bytes(distance);

    out[strm->out_pos] |= (match_len_bytes << 3);
    out[strm->out_pos] |= distance_bytes;
    uint8_t move_len = match_len_bytes + distance_bytes;
    uint8_t tmp_match_len_bytes = match_len_bytes;

    // 长度
    while (match_len_bytes > 0) {
        out[strm->out_pos + match_len_bytes] = match_len & 0xff;
        match_len_bytes--;
        match_len = match_len >> 8;
    }

    // 距离
    while (distance_bytes > 0) {
        out[strm->out_pos + tmp_match_len_bytes + distance_bytes] = match_len & 0xff;
        distance_bytes--;
        distance = distance >> 8;
    }
    strm->out_pos += move_len + 1;

    return tmp_match_len;
}

static int lz_encode_stream(lz_compressor_t *comp, lz_stream_t *strm, uint32_t *anchor)
{
    uint32_t seq = lz_read_seq(strm->in + strm->in_pos);

    lz_backward_t *backward = lz_get_backward(comp->backward_dict, seq);
    if (backward == NULL) {
        lz_createorset_backward(comp->backward_dict, seq, strm->in_pos);
        strm->in_pos++;
        return TOY_ERR_LZ_BACKWARD_NOT_EXIST;
    }

    lz_encode_literals(strm, anchor, refpos->pos);
    uint32_t match_len = 
    lz_encode_match(strm, refpos->pos);

    // anchor move
    anchor = strm->in_pos + match_len;

    return TOY_OK;
}

static int lz_start_compress(lz_compressor_t *comp)
{
    lz_stream_t *strm = &comp->strm;

    uint32_t anchor = 0;
    while (strm->in_pos < strm->in_size - SEQ_SIZE) {
        int ret = lz_encode_stream(comp, strm, &anchor);
        if (ret == TOY_ERR_LZ_BACKWARD_NOT_EXIST) {
            continue;
        }
        if (ret != TOY_OK) {
            return ret;
        }
    }
    
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

    lz_init_strm(strm);

    int ret = lz_start_compress(comp);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

static bool lz_is_literals_token(lz_stream_t *strm)
{
    uint8_t token = *(uint8_t *)strm->in;

    if (token & 128 > 0) {
        return false;
    }
    return true;
}

static int lz_decompress_literals(lz_stream_t *strm)
{
    uint32_t literal_len = 0;
    uint8_t token = *(uint8_t *)strm->in;

    literal_len = token & 0x1f;
    uint8_t literal_len_byte = (token >> 5) & 0x3;
 
    while (literal_len_byte > 0) {
        strm->in_pos += 1;
        literal_len = (literal << 8) + strm->in[strm->in_pos];
        literal_len_byte--;
    }
    strm->in_pos;
    memcpy(strm->out + strm->out_pos, strm->in + strm->in_pos, literal_len);
    strm->in_pos += literal_len;
    return 0;
}

static int lz_decompress_match(lz_stream_t *strm)
{
    uint8_t token = *(uint8_t *)strm->in;
    uint8_t len_bytes = (token >> 3) & 0x7;
    uint8_t dist_bytes = token & 0x7;

    strm->in_pos++;
    uint32_t len = 0;
    while (len_bytes > 0) {
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

    memcpy(strm->out + strm->out_pos, strm->in + strm->in_pos - dist, len);
    strm->out_pos += len;
    return 0;
}

static int lz_start_decompress(lz_compressor_t *comp)
{
    lz_stream_t *strm = &comp->strm;

    if (lz_is_literals_token(strm)) {
        return lz_decompress_literals(strm);
    } else {
        return lz_decompress_match(strm);
    }
    return TOY_OK;
}

int lz_decompress(lz_compressor_t *comp, lz_stream_t *strm)
{
    if (strm->in_size < MIN_INPUT_LEN) {
        return TOY_ERR_LZ_INVALID_PARA;
    }

    lz_init_strm(strm);

    return lz_start_decompress(comp);
}