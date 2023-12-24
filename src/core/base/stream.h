
/* ********************************
 * Author:       Zhanglele
 * Description:  文件流操作
 * create time:  2023.10.29
 ********************************/
#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>
#include "pub_def.h"

/* STREAM模块错误码，基址为500 */
#define TOY_ERR_STREAM_WRITE_OVER_FLOW (TOY_ERR_STREAM_MODULE_BASE + 0) 
#define TOY_ERR_STREAM_READ_OVER_FLOW  (TOY_ERR_STREAM_MODULE_BASE + 1) 

typedef struct {
    uint8_t *data;      // 数据流
    uint32_t size;      // 数据流长度
    uint32_t pos;       // 数据流偏移
    uint8_t bit_pos;    // bit偏移
} stream_t;


/** 创建字节流对象
 *  @param: data: [in] 数据
 *  @param: size: [in] 数据长度
 *  @return：失败返回NULL
 */
stream_t *stream_create(char *data, uint32_t size);

/** 释放字节流
 *  @param: strm: [in] 字节流对象
 */
void stream_free(stream_t *strm);

/** 字节流写bit值
 *  @param: strm: [in] 字节流对象
 *  @param: bit:  [in] bit值，0 or 1
 *  @return：成功 或者 失败
 */
int stream_write_bit(stream_t *strm, uint8_t bit);

/** 字节流写bit值
 *  @param: strm: [in] 字节流对象
 *  @return：bit值，0 or 1
 */
int stream_read_bit(stream_t *strm);

#endif