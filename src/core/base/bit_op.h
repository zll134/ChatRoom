
/* ********************************
 * Author:       Zhanglele
 * Description:  位操作
 * create time:  2023.07.30
 ********************************/
#ifndef BIT_OP_H
#define BIT_OP_H

#include <stdint.h>

/** 获取整数存储需要的字节数
 *  @param: val: [in] 输入整数
 *  @return 返回整数存储需要的字节数
 */
uint8_t bit_get_bytes(uint32_t val);

#endif