/* ********************************
 * Author:       Zhanglele
 * Description:  压缩后向引用管理
 * create time:  2023.07.30
 ********************************/
#ifndef LZ_BACKWARD_H
#define LZ_BACKWARD_H

#include <stdint.h>

#include "dict.h"

dict_t *lz_create_backward_ref_dict();

void lz_destroy_backward_ref_dict(dict_t *dict);

int lz_insert_backward_ref(dict_t *dict, uint32_t seq, uint32_t refpos);

/** 获取引用位置
 *  @param: dict: [in] 后向引用位置字典
 *  @param: seq: [in] 字节流头四字节值
 *  @param: refpos: [out] 所有可用的引用位置
 *  @return 引用位置数量。 0表示没有引用位置
 */
uint32_t lz_get_backward_refs(dict_t *dict, uint32_t seq, uint32_t **refposes);

#endif