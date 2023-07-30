/* ********************************
 * Author:       Zhanglele
 * Description:  压缩后向引用管理
 * create time:  2023.07.30
 ********************************/
#ifndef LZ_BACKWARD_H
#define LZ_BACKWARD_H

#include <stdint.h>
#include "dict.h"

typedef struct {
    uint32_t seq;    // 四字节序列
    uint32_t refpos; // 引用位置
} lz_backward_t;

dict_t *lz_create_backward_dict();

void lz_destroy_backward_dict(dict_t *dict);

int lz_createorset_backward(dict_t *dict, uint32_t seq, uint32_t refpos);

lz_backward_t *lz_get_backward(dict_t *dict, uint32_t seq);

#endif