/* ********************************
 * Author:       Zhanglele
 * Description:  环形缓冲区管理
 * create time:  2023.08.27
 ********************************/
#ifndef RINGBUFF_H
#define RINGBUFF_H

#include <stdint.h>

typedef struct {
    uint8_t *buff;      /* 缓存空间起始地址 */
    uint32_t buff_size;  /* 缓存空间大小 */
    uint64_t logic_pos; /* 逻辑位置*/
} ringbuff_t;

typedef struct {
    uint8_t buff_size; /* 环形缓冲区大小 */
} ringbuff_option_t;

/** 创建环形缓冲区对象
 * 
 *  @param: option: [in] 环形缓冲区参数
 *  @return: NULL表示创建环形缓冲区失败
 */
ringbuff_t *ringbuff_create(ringbuff_option_t *option);

void ringbuff_destroy(ringbuff_t *ringbuff);

int ringbuff_write(ringbuff_t *ringbuff, uint8_t *bytes, uint32_t nbyte);

#endif