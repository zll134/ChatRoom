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
#define TOY_ERR_HUFFMAN_INDEX_OVERFLOW          (TOY_ERR_HUFFMAN_MODULE_BASE + 7)

/** huffman 编码
 *  @attention 出参out->data需要由调用者释放
 *
 *  @param: in: [in] 输入字节流
 *  @param: out: [out] 输出字节流;
 *  @return: TOY_OK表示压缩成功；失败返回对应错误码
 */
int huffman_encode(stream_t *in, stream_t *out);

/** huffman 解码
 *  @attention 出参out->data需要由调用者释放
 *
 *  @param: in: [in] 输入字节流
 *  @param: out: [out] 输出字节流;
 *  @return: TOY_OK表示压缩成功；失败返回对应错误码
 */
int huffman_decode(stream_t *in, stream_t *out);

#endif