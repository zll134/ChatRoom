/* ********************************
 * Author:       Zhanglele
 * Description:  压缩算法
 * create time:  2024.02.25
 ********************************/
#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>

#include "pub_def.h"
#include "toylz.h"
#include "stream.h"

#define MIN_COMPRESS_LEVEL 0
#define MAX_COMPRESS_LEVEL 9

#define MAX_DECOMPRESS_LEN (1024 * 512)

typedef struct {
    int level;           // 压缩级别，范围为0~9；0速度最快，9的压缩比最高
} compressor_option_t;

typedef struct {
    lz_compressor_t *lz_compressor;
} compressor_t;

compressor_t *compressor_create(compressor_option_t *option);

void compressor_destroy(compressor_t *compressor);

int compressor_encode(compressor_t *compressor, stream_t *in, stream_t *out);

int compressor_decode(compressor_t *compressor, stream_t *in, stream_t *out);

#endif