/* ********************************
 * Author:       Zhanglele
 * Description:  hash函数
 * create time:  2023.06.10
 ********************************/
#include "hash.h"

#include <stdint.h>

uint32_t dict_int_hash_func(uint32_t key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}
