/* ********************************
 * Author:       Zhanglele
 * Description:  长整数
 * create time:  2023.10.29
 ********************************/
#include "big_int.h"
#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "pub_def.h"
/*

一、长整数结构体

设计原则：

byte低字节存储低位数据
举例：
    1  <->  0x1
    +-----+
    | 0x1 |
    +-----+

    600  <-> 0x258
    +------+------+
    | 0x58 | 0x02 |
    +------+------+

    8855553  <-> 0x872001
    +------+------+------+
    | 0x01 | 0x20 | 0x87 |
    +------+------+------+
*/

#define BYTE_BITS 8
#define BYTE_MAX_VAL 256

long_integer_t *integer_new(uint64_t val, uint32_t byte_len)
{
    if (byte_len == 0) {
        return NULL;
    }

    long_integer_t *integer = calloc(1, sizeof(long_integer_t));
    if (integer == NULL) {
        return NULL;
    }

    integer->bytes = calloc(1, (byte_len * sizeof(uint8_t)));
    if (integer->bytes == NULL) {
        free(integer);
        return NULL;
    }

    // 主机序转换为小端序进行存储
    val = htole64(val);

    // 初始化bytes数组
    integer->byte_len = byte_len;
    for (int i = 0; i < byte_len; i++) {
        integer->bytes[i] = (val & 0xff);
        val = (val >> BYTE_BITS);
    }

    return integer;
}

void integer_free(long_integer_t *integer)
{
    if (integer != NULL) {
        free(integer->bytes);
        free(integer);
    }
}

void integer_shift_left(long_integer_t *integer, uint32_t bits)
{
    uint8_t *vals = integer->bytes;
    uint32_t len = integer->byte_len;
    for (int i = len - 1; i >= 0; i--) {
        uint8_t val = vals[i];
        vals[i] = (vals[i] << bits);

        uint32_t byte_pos = (bits / BYTE_BITS) + i;
        uint8_t bit_pos = (bits % BYTE_BITS);
    
        // 超出边界值则处理下一个
        if (byte_pos >= len) {
            continue;
        }
    
        // 位于边界值上
        if (byte_pos == len - 1) {
            vals[byte_pos] = (vals[byte_pos] & ((1 << bit_pos) - 1)) + (val << bit_pos);
            TRACE("[big_int] left shift, idx %d, byte[%d] %u",
                i, byte_pos, vals[byte_pos]);
        } else {
            vals[byte_pos] = (vals[byte_pos] & ((1 << bit_pos) - 1)) + (val << bit_pos);
            vals[byte_pos + 1] = (vals[byte_pos + 1] & ((0xff) << bit_pos)) +
                       (val >> (BYTE_BITS - bit_pos));
            TRACE("[big_int] left shift, idx %d, byte[%d] %u, byte[%d] %u",
                i, byte_pos, vals[byte_pos], byte_pos + 1, vals[byte_pos + 1]);
        } 
    }
}

void integer_shift_right(long_integer_t *integer, uint32_t bits)
{
    uint8_t *vals = integer->bytes;
    uint32_t len = integer->byte_len;

    for (int i = 0; i < len; i++) {
        uint32_t byte_pos = (bits / BYTE_BITS) + i;
        uint8_t bit_pos = (bits % BYTE_BITS);

        // 超出边界值则处理下一个
        if (byte_pos >= len) {
            vals[i] = 0;
            continue;
        }

        if (byte_pos == len - 1) {
            vals[i] = (vals[byte_pos] >> bit_pos);
            TRACE("[big_int] Right shift, idx %d, byte[%d] %u",
                i, byte_pos, vals[byte_pos]);
        } else {
            vals[i] = (vals[byte_pos] >> bit_pos) | (vals[byte_pos + 1] << (BYTE_BITS - bit_pos));
        } 
    }
}

void integer_add(long_integer_t *integer, uint32_t val)
{
    // 主机序转换为小端序进行存储
    val = htole32(val);

    uint32_t len = integer->byte_len;
    int i = 0;
    int carry = val; // 进位
    while (i < len) {
        int sum = integer->bytes[i] + carry;
        carry = sum / BYTE_MAX_VAL;
        integer->bytes[i] = sum % BYTE_MAX_VAL;
        TRACE("[big_int] Bigint add, idx %d, carry %d, byte %u",
                i, carry, integer->bytes[i]);
        i++;
    }
}

uint8_t integer_get_bit(long_integer_t *integer, uint32_t index)
{
    uint32_t byte_pos = (index / BYTE_BITS);
    uint8_t bit_pos = (index % BYTE_BITS);

    if (index > (BYTE_BITS * integer->byte_len - 1)) {
        return MAX_UINT8;
    }
    if ((integer->bytes[byte_pos] & (1 << bit_pos)) == 0) {
        return 0;
    }
    return 1;
}

void integer_inc(long_integer_t *integer)
{
    integer_add(integer, 1);
}

void integer_set(long_integer_t *integer, uint32_t val)
{
    // 主机序转换为小端序进行存储
    val = htole32(val);

    for (int i = 0; i < integer->byte_len; i++) {
        integer->bytes[i] = (val & 0xff);
        val = (val >> BYTE_BITS);
    }
}

bool integer_equal(long_integer_t *integer1, long_integer_t *integer2)
{
    long_integer_t *long_int = NULL;
    long_integer_t *short_int = NULL;

    if (integer1->byte_len >= integer2->byte_len) {
        long_int = integer1;
        short_int = integer2;
    } else {
        long_int = integer2;
        short_int = integer1;
    }

    for (int i = long_int->byte_len - 1; i > short_int->byte_len; i--) {
        if (long_int->bytes[i] != 0) {
            return false;
        }
    }

    for (int i = short_int->byte_len - 1; i >= 0; i--) {
        TRACE("[big_int] Bigint bytes compare, idx %d, lbyte %u, sbyte %u",
                i, long_int->bytes[i], short_int->bytes[i]);

        if (long_int->bytes[i] != short_int->bytes[i]) {
            return false;
        }
    }

    return true;
}

int integer_copy(long_integer_t *integer, long_integer_t *val)
{
    if ((integer == NULL) || (val == NULL)) {
        return TOY_ERR_BIG_INT_INVALID_PARA;
    }

    uint32_t byte_len = integer->byte_len < val->byte_len ? integer->byte_len : val->byte_len;
    memcpy(integer->bytes, val->bytes, byte_len * sizeof(uint8_t));
    return TOY_OK;
}