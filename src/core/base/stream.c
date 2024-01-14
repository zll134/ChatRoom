
/* ********************************
 * Author:       Zhanglele
 * Description:  文件流操作
 * create time:  2023.10.29
 ********************************/

#include "stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pub_def.h"
#include "log.h"
#define BYTE_BITS 8

stream_t *stream_create(char *data, uint32_t size)
{
    stream_t *strm = calloc(1, sizeof(*strm));
    if (strm == NULL) {
        return NULL;
    }

    strm->data = calloc(1, size);
    if (strm->data == NULL) {
        free(strm);
        return NULL;
    }

    if (data != NULL) {
        memcpy(strm->data, data, size);
    }

    strm->size = size;
    return strm;
}

void stream_free(stream_t *strm)
{
    if (strm == NULL) {
        return;
    }

    if (strm->data != NULL) {
        free(strm->data);
        strm->data = NULL;
        strm->size = 0;
        strm->pos = 0;
    }

    free(strm);

}

int stream_write_bit(stream_t *strm, uint8_t bit)
{
    uint32_t byte_pos = strm->pos;
    uint8_t bit_pos = strm->bit_pos;
    if (byte_pos >= strm->capacity) {
        ERROR("[stream] Failed to write bit due to overflow, off %u, size %u.",
            byte_pos, strm->capacity);
        return TOY_ERR_STREAM_WRITE_OVER_FLOW;
    }

    if (bit == 0) {
        strm->data[strm->pos] &= (~(1 << bit_pos));
    } else {
        strm->data[strm->pos] |= (1 << bit_pos);
    }
    strm->bit_pos++;

    if (strm->bit_pos >= BYTE_BITS) {
        strm->pos += 1;
        strm->bit_pos = (strm->bit_pos % BYTE_BITS);
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

    int bit = 0;
    if (strm->data[strm->pos] & (1 << bit_pos)) {
        bit = 1;
    }

    strm->bit_pos++;
    if (strm->bit_pos >= BYTE_BITS) {
        strm->pos += 1;
        strm->bit_pos = strm->bit_pos % BYTE_BITS;
    }

    return bit;
}