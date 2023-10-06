/* ********************************
 * Author:       Zhanglele
 * Description:  huffman算法
 * create time:  2023.08.27
 ********************************/
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>

int huffman_encode(uint8_t *in, uint32_t in_len, uint8_t **out, uint32_t *out_len);

int huffman_decode(uint8_t *in, uint32_t in_len, uint8_t **out, uint32_t *out_len);
#endif