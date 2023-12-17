/* ********************************
 * Author:       Zhanglele
 * Description:  huffman算法
 * create time:  2023.08.27
 ********************************/
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>

#include "pub_def.h"
#include "stream.h"

/* HUFFMAN模块错误码，基址为400 */
#define TOY_ERR_HUFFMAN_INVALID_PARA            (TOY_ERR_HUFFMAN_MODULE_BASE + 0)
#define TOY_ERR_HUFFMAN_MALLOC_FAILED           (TOY_ERR_HUFFMAN_MODULE_BASE + 1)
#define TOY_ERR_HUFFMAN_NEW_NOSYM_NODE_FAILD    (TOY_ERR_HUFFMAN_MODULE_BASE + 2)
#define TOY_ERR_HUFFMAN_SYMBOL_OVERFLOW         (TOY_ERR_HUFFMAN_MODULE_BASE + 3)
#define TOY_ERR_HUFFMAN_NUMBITS_OVERFLOW        (TOY_ERR_HUFFMAN_MODULE_BASE + 4)
#define TOY_ERR_HUFFMAN_OUTBUF_INSUFFICIET      (TOY_ERR_HUFFMAN_MODULE_BASE + 5)
#define TOY_ERR_HUFFMAN_NEW_BIG_INT             (TOY_ERR_HUFFMAN_MODULE_BASE + 6)

int huffman_encode(stream_t *in, stream_t *out);

int huffman_decode(stream_t *in, stream_t *out);

int huffman_parse(stream_t *encode_strm);

#endif