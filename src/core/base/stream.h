
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
    uint8_t bit_off;    // bit偏移
} stream_t;

/** 字节流写bit值
 *  @param: strm: [in] 字节流对象
 *  @param: bit:  [in] bit值，0 or 1
 *  @return：成功 或者 失败
 */
int stream_write_bit(stream_t *strm);

#endif