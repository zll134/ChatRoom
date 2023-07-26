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
    int level;           // 压缩级别，范围为0~9
} lz_options_t;

typedef struct {
    uint8_t *stream;
    uint32_t cur_pos;
    uint32_t total_len;
} lz_stream_t;

typedef struct {
    lz_stream_t in;      // 输入流
    lz_stream_t out;     // 输出流
    uint32_t block_size;  // 块大小
} lz_compressor_t;

/** 预先设置压缩时的压缩参数
 *  @param: option: [out] 表示压缩参数
 *          level:  [in]  表示压缩级别，范围为0~9
 *  @return: TOY_OK 表示成功
 */
int lz_option_preset(lz_options_t *option, int level);
lz_compressor_t *lz_create_compressor(lz_options_t *option);
int lz_compress(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t out_len,
    lz_options_t *option);

#endif