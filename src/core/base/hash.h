/* ********************************
 * Author:       Zhanglele
 * Description:  hash函数
 * create time:  2023.06.10
 ********************************/
#ifndef HASH_H
#define HASH_H

#include <stdint.h>

/* Thomas Wang's 32 bit Mix Function */
uint32_t dict_int_hash_func(uint32_t key);

#endif