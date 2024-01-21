/* ********************************
 * Author:       Zhanglele
 * Description:  huffman算法
 * create time:  2023.08.27
 ********************************/
#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>

#include "pub_def.h"

#define MIN_COMPRESS_LEVEL 0
#define MAX_COMPRESS_LEVEL 9

typedef struct {
    int level;           // 压缩级别，范围为0~9
} compressor_option_t;

typedef struct {
    uint32_t sliding_win;  // 滑窗大小
    dict_t *backward_refs;  // 前向引用字典
} compressor_t;

compressor_t *compressor_create(lz_option_t *option);

void compressor_destroy(compressor_t *compressor);

int compressor_encode();

int compressor_decode(stream_t *in, stream_t *out);

#endif