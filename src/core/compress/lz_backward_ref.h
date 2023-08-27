/* ********************************
 * Author:       Zhanglele
 * Description:  压缩后向引用管理
 * create time:  2023.07.30
 ********************************/
#ifndef LZ_BACKWARD_H
#define LZ_BACKWARD_H

#include <stdint.h>

#include "dict.h"
#include "ringbuff.h"

typedef struct {
    uint32_t seq;    // 四字节序列
    ringbuff_t *refs; // 所有引用位置
} lz_backward_ref_t;

dict_t *lz_create_backward_ref_dict();

void lz_destroy_backward_ref_dict(dict_t *dict);

int lz_insert_backward_ref(dict_t *dict, uint32_t seq, uint32_t refpos);

lz_backward_ref_t *lz_get_backward_ref(dict_t *dict, uint32_t seq);

#endif