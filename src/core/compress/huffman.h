/* ********************************
 * Author:       Zhanglele
 * Description:  huffman算法
 * create time:  2023.08.27
 ********************************/
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include "stream.h"

int huffman_encode(stream_t *in, stream_t *out);

int huffman_decode(stream_t *in, stream_t *out);

int huffman_free_stream(stream_t *strm);
#endif