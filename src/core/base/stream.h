
/* ********************************
 * Author:       Zhanglele
 * Description:  文件流操作
 * create time:  2023.10.29
 ********************************/
#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>

typedef struct {
    uint8_t *data;      // 数据流
    uint32_t size;      // 数据流长度
    uint32_t pos;       // 数据流偏移
} stream_t;

#endif