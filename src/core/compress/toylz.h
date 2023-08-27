/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/
#ifndef TOYLZ_H
#define TOYLZ_H

#include <stdint.h>
#include "dict.h"

#define LZ_MIN_COMPRESS_LEVEL 0
#define LZ_MAX_COMPRESS_LEVEL 9

typedef struct {
    int level;           // 压缩级别，范围为0~9
} lz_option_t;

typedef struct {
    uint8_t *in;         // 输入字节流
    uint32_t in_size;    // 输入字节流长度
    uint32_t in_pos;     // 输入字节流中的位置
    uint8_t *out;        // 输出字节流
    uint32_t out_size;   // 输出字节流的空间大小
    uint32_t out_pos;    // 输出字节流中的位置
    uint32_t out_total;  // 输出字节流的长度
} lz_stream_t;

typedef struct {
    uint32_t sliding_win;  // 滑窗大小
    dict_t *backward_refs;  // 前向引用字典
} lz_compressor_t;

/** 创建压缩器
 *  @param: option: [in] 表示压缩参数
 *  @return: 压缩器对象; NULL 表示创建失败.
 */
lz_compressor_t *lz_create_compressor(lz_option_t *option);

/** 销毁压缩器
 *  @param: comperssor: [in] 压缩器对象
 *  @return: NULL 表示创建失败
 */
void lz_destroy_compressor(lz_compressor_t *compressor);

/** 字节流压缩
 *  @param: comp: [in] 压缩器对象
 *  @param: strm: [in] 输入与输出字节流
 *  @return: TOY_OK表示压缩成功
 */
int lz_compress(lz_compressor_t *comp, lz_stream_t *strm);

/** 字节流解压
 *  @param: comp: [in] 压缩器对象
 *  @param: strm: [in] 输入与输出字节流
 *  @return: TOY_OK表示解压成功
 */
int lz_decompress(lz_compressor_t *comp, lz_stream_t *strm);

#endif