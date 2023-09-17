/* ********************************
 * Author:       Zhanglele
 * Description:  位操作
 * create time:  2023.07.30
 ********************************/
#include "bit_op.h"

uint8_t bit_get_bytes(uint32_t val)
{
    uint8_t bytes = 0;
    while (val != 0) {
        val = val >> 8;
        bytes++;
    }
    return bytes;
}