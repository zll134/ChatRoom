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
 *  @param: option: [in] 环形缓冲区参数
 *  @return: 返回NULL，表示创建环形缓冲区失败
 */
ringbuff_t *ringbuff_create(ringbuff_option_t *option);

/** 销毁环形缓冲区对象
 *  @param: ringbuff: [in] 环形缓冲区对象
 */
void ringbuff_destroy(ringbuff_t *ringbuff);

/** 往环形缓冲区写入输入流
 *  @attention: 超出环形缓冲区的内容将覆盖数据
 * 
 *  @param: ringbuff: [in] 环形缓冲区对象
 *  @param: bytes: [in] 输入流指针
 *  @param: nbyte: [in] 输入流长度
 *  @return: TOY_OK 表示写成功; 
 */
int ringbuff_write(ringbuff_t *ringbuff, uint8_t *bytes, uint32_t nbyte);

/** 读取环形缓冲区内容
 *  @param: ringbuff: [in] 环形缓冲区对象
 *  @param: bytes: [in] 输入流指针
 *  @param: nbyte: [in] 输入流长度
 *  @return 返回值为读取字节的大小
 */
int ringbuff_read(ringbuff_t *ringbuff, uint8_t *bytes, uint32_t nbyte);

/** 环形缓冲区中内容长度
 *  @param: ringbuff: [in] 环形缓冲区对象
 */
uint32_t ringbuff_len(ringbuff_t *ringbuff);

#endif