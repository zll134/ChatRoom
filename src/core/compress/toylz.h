/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/
#ifndef TOYLZ_H
#define TOYLZ_H

#include <stdint.h>

#define LZ_MIN_COMPRESS_LEVEL 0
#define LZ_MAX_COMPRESS_LEVEL 9

typedef struct {
    uint32_t sliding_win_size;
} lz_options_t;

/** 预先设置压缩时的压缩参数
 *  @param: option: [out] 表示压缩参数
 *          level:  [in]  表示压缩级别，范围为0~9
 *  @return: TOY_OK 表示成功
 *           
 */
int lz_option_preset(lz_options_t *option, int level);

int lz_decompress(const void* input, int length, void* output, int maxout);

int lz_compress(const void *in, uint32_t in_len, void *out, uint32_t out_len,
    lz_options_t *option);

#endif