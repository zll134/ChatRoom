/* ********************************
 * Author:       Zhanglele
 * Description:  环形缓冲区管理
 * create time:  2023.08.27
 ********************************/
#include "ringbuff.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pub_def.h"
#include "log.h"

ringbuff_t *ringbuff_create(ringbuff_option_t *option)
{
    ringbuff_t *ringbuff = calloc(1, sizeof(*ringbuff));
    if (ringbuff == NULL) {
        diag_err("[ringbuff] Create ringbuff obj failed.");
        return NULL;
    }

    ringbuff->buff_size = option->buff_size;
    ringbuff->buff = calloc(1, ringbuff->buff_size);
    if (ringbuff->buff == NULL) {
        diag_err("[ringbuff] Create ringbuff buffer failed.");
        return NULL;
    }

    return ringbuff;
}

void ringbuff_destroy(ringbuff_t *ringbuff)
{
    if (ringbuff != NULL) {
        free(ringbuff->buff);
        free(ringbuff);
    }
}

int ringbuff_write(ringbuff_t *ringbuff, uint8_t *bytes, uint32_t nbyte)
{
    if (bytes == NULL || nbyte == 0) {
        diag_err("[ringbuff] Ringbuff write para invalid.");
        return TOY_ERR_RINGBUFF_WRITE_INVALID_PARA;
    }

    uint8_t *buff = ringbuff->buff;
    uint32_t buff_size = ringbuff->buff_size;
    uint32_t real_pos = ringbuff->logic_pos % buff_size;

    if (real_pos + nbyte < ringbuff->buff_size) {
        memcpy(&buff[real_pos], bytes, nbyte);
    } else {
        /* 临界位置分成两个位置写入 */
        memcpy(&buff[real_pos], bytes, buff_size - real_pos);
        memcpy(&buff[0], bytes + (buff_size - real_pos),
            nbyte - (buff_size - real_pos));
    }

    ringbuff->logic_pos += nbyte;

    return TOY_OK;
}

int ringbuff_read(ringbuff_t *ringbuff, uint8_t *bytes, uint32_t nbyte)
{
    if ((ringbuff == NULL) || (bytes == NULL)) {
        diag_err("[ringbuff] Ringbuff read para invalid.");
        return -1;
    }

    uint8_t *buff = ringbuff->buff;
    uint32_t buff_size = ringbuff->buff_size;

    uint32_t read_bytes = nbyte < ringbuff_len(ringbuff) ? nbyte : ringbuff_len(ringbuff);

    uint32_t start_pos = (ringbuff->logic_pos - read_bytes) % buff_size;
    uint32_t end_pos = ringbuff->logic_pos % buff_size;

    if (start_pos >= end_pos) {
        memcpy(bytes, &buff[start_pos], buff_size- start_pos);
        memcpy(&bytes[buff_size- start_pos], &buff[0], end_pos);
    } else {
        memcpy(&bytes[0], &buff[start_pos], end_pos - start_pos);
    }

    return read_bytes;
}

uint32_t ringbuff_len(ringbuff_t *ringbuff)
{
    if (ringbuff->logic_pos < ringbuff->buff_size) {
        return (uint32_t)ringbuff->logic_pos;
    }

    return ringbuff->buff_size;
}