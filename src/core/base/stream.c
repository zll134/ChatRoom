
/* ********************************
 * Author:       Zhanglele
 * Description:  文件流操作
 * create time:  2023.10.29
 ********************************/

#include "stream.h"

#include "pub_def.h"

#define BYTE_BITS 8

int stream_write_bit(stream_t *strm, uint8_t bit)
{
    uint32_t byte_pos = strm->pos;
    uint8_t bit_pos = strm->bit_pos;
    if (byte_pos >= strm->size) {
        return TOY_ERR_STREAM_WRITE_OVER_FLOW;
    }

    if (bit == 0) {
        strm->data[bit_pos] &= (~(1 << bit_pos));
    } else {
        strm->data[bit_pos] |= ((1 << bit_pos);
    }
    strm->bit_pos++;

    if (strm->bit_pos >= BYTE_BITS) {
        strm->pos += 1;
        strm->bit_pos = strm->bit_pos % BYTE_BITS;
    }

    return TOY_OK;
}

int stream_read_bit(stream_t *strm)
{
    uint32_t byte_pos = strm->pos;
    uint8_t bit_pos = strm->bit_pos;
    if (byte_pos >= strm->size) {
        return TOY_ERR_STREAM_READ_OVER_FLOW;
    }

    if (strm->data[strm->pos] & (1 << bit_pos)) {
        return 1;
    }
    return 0;
}