/* ********************************
 * Author:       Zhanglele
 * Description:  huffman算法
 * create time:  2023.08.27
 ********************************/
#include "huffman.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pub_def.h"
#include "bit_op.h"
#include "stream.h"
/*
1. huffman 压缩格式
    格式：
        header + 码表 + 字符编码
        +========+======+=========+
        | header | 码表 | 字符编码 |
        +========+======+=========+
2. header
    +----+----+----+----+
    |        size       |
    +----+----+----+----+

3. 码表
    码表的格式参考rfc1951，存储所有字符的长度的游程编码。
    码表存储每个字符的编码长度
    3.1 每个字符的存储格式：
        len - num
        如果num为1，格式：
            +-------+
            | 1 len |
            +-------+
        如果num大于1，格式
            +-------+-----+
            | 0 len | num |
            +-------|-----+
4. 字符编码
    字符编码使用huffman编码。

*/

#define MAX_SYMBOL_NUM 256
#define MAX_BITS_LEN 255
#define MAX_BYTE_LEN 32

typedef struct huffman_node_tag
{
    struct huffman_node_tag *parent;
    uint32_t count;
    bool is_leaf;
    union {
        struct {
            struct huffman_node_tag *zero;
            struct huffman_node_tag *one;
        };
        uint8_t symbol;
    };
    uint32_t height;
} huffman_node_t;

typedef struct huffman_code_tag {
    uint32_t numbits;
    uint8_t bits[MAX_BYTE_LEN];
} huffman_code_t;

/* byte length */
typedef struct {
    uint32_t count;
    huffman_code_t start_code;
} huffman_bl_t;

static huffman_node_t *huffman_new_sym_node(uint8_t symbol)
{
    huffman_node_t *node = calloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }
    node->is_leaf = true;
    node->parent = NULL;
    node->symbol = symbol;
    node->count = 0;
    node->height = 0;
    return node;
}

static int huffman_calc_symbol_frequencies(huffman_node_t **sym_nodes, uint32_t len,
    uint8_t *in, uint32_t in_len)
{
    int total_count = 0;
    for (uint32_t i = 0; i < in_len; i++) {
        uint8_t symbol = in[i];
        if (!sym_nodes[symbol]) {
            sym_nodes[symbol] = huffman_new_sym_node(symbol);
            if (sym_node[symbol] == NULL) {
                return TOY_ERR_HUFFMAN_MALLOC_FAILED;
            }
        }

        sym_nodes[symbol]->count++;
    }

    return TOY_OK;
}

static void huffman_free_tree(huffman_node_t *sym_node)
{
    if (sym_node == NULL) {
        return;
    }
    if (sym_node->is_leaf) {
        free(sym_node);
        return;
    }

    huffman_free_tree(sym_node->zero);
    huffman_free_tree(sym_node->one);

    free(sym_node->zero);
    sym_node->zero = NULL;
    free(sym_node->one);
    sym_node->one = NULL;
}

static void huffman_free_sym_nodes(huffman_node_t **sym_nodes, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (sym_nodes[i] != NULL) {
            huffman_free_tree(sym_nodes[i]);
            sym_nodes[i] = NULL;
        }
    }
}

static int huffman_sym_node_compare(const void* p1, const void* p2)
{
    const huffman_node_t *node1 = *(const huffman_node_t**)p1;
    const huffman_node_t *node2 = *(const huffman_node_t**)p2;

    if (node1 == NULL && node2 == NULL) {
        return 0;
    }

    if (node1 == NULL) {
        return 1;
    }
	
    if (node2 == NULL) {
        return -1;
    }
		
    if (node1->count > node2->count) {
        return 1;
    } else if (node1->count < node2->count) {
        return -1;
    }

    return 0;
}

static huffman_node_t *huffman_new_nosym_node(uint8_t count,
    huffman_node_t *zero, huffman_node_t *one)
{
    huffman_node_t *node = calloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }
    node->is_leaf = false;
    node->parent = NULL;
    node->zero = zero;
    node->one = one;
    node->count = count;
    node->height = 0;
    return node;
}

/* 构建霍夫曼树 */
static int huffman_build_tree(huffman_node_t **sym_nodes, uint32_t len)
{
    /* 按照频率排序 */
	qsort(sym_nodes, len, sizeof(sym_nodes[0]), huffman_sym_node_compare);

    /* 获取符号的个数 */
    uint32_t count = 0;
	for (int i = 0; i < MAX_SYMBOL_NUM; i++) {
        if (sym_nodes[i] != NULL) {
            count++;
        }
    }

	for (int i = 1; i < count; i++) {
        huffman_node_t *m1 = sym_nodes[0];
        huffman_node_t *m2 = sym_nodes[1];

        /* 将m1和m2合并成新的节点 */
        huffman_node_t *node = huffman_new_nosym_node(m1->count + m2->count, m1, m2);
        if (node == NULL) {
            return TOY_ERR_HUFFMAN_NEW_NOSYM_NODE_FAILD;
        }
        m1->parent = node;
        m2->parent = node;
        sym_nodes[0] = node;
        sym_nodes[1] = NULL;

        /* 重新排序 */
        qsort(sym_nodes, count, sizeof(psym_nodes[0]), huffman_sym_node_compare);
	}
    return TOY_OK;
}

static int huffman_get_sym_numbits(huffman_node_t *root,
    huffman_code_t *codes, uint32_t len)
{
    if (root == NULL) {
        return TOY_OK;
    }

    if (root->parent != NULL) {
        root->height = root->parent + 1;
    }

    /* 符号叶节点处理 */
    if (root->is_leaf) {
        if (root->symbol >= len) {
            return TOY_ERR_HUFFMAN_SYMBOL_OVERFLOW;
        }
        codes[root->symbol].numbits = root->height;
        return TOY_OK;
    }

    return (huffman_get_sym_numbits(root->zero, codes, len) ||
        huffman_get_sym_numbits(root->one, codes, len));
}

static int huffman_get_bl_counts(const huffman_code_t *codes, uint32_t codes_len,
    huffman_bl_t **bl_count, uint32_t *len)
{
    huffman_bl_t *bl = calloc(MAX_BITS_LEN + 1, sizeof(bl_count));
    if (bl == NULL) {
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    for (int i = 0; i < codes_len; i++) {
        if (codes[i].numbits == 0) {
            continue;
        }
        if (codes[i].numbits >= len) {
            return TOY_ERR_HUFFMAN_NUMBITS_OVERFLOW;
        }
        bl[codes[i].numbits].count++;
    }

    for (int i = 1; i < MAX_BITS_LEN + 1; i++) {
        bl[i].start_code.numbits = i;
        memcpy(bl[i].start_code.bits, bl[i - 1].start_code.bits, 
            sizeof(bl[i - 1].start_code.bits));
        bit_add(bl[i].start_code.bits, MAX_BYTE_LEN, bl[i - 1].count);
        bit_shift_left(bl[i].start_code.bits, MAX_BYTE_LEN, 1);
    }

    *bl_count = bl;
    *len = MAX_BITS_LEN + 1;
    return TOY_OK;
}

static int huffman_build_sym_bits(const huffman_bl_t *bl_counts, uint32_t bl_len,
    huffman_code_t *codes, uint32_t codes_len)
{
    for (int i = 0; i < codes_len; i++) {
        uint32_t numbits = codes[i].numbits;
        huffman_code_t *src_code = bl_counts[numbits].start_code;
        memcpy(codes[i].bits, src_code->bits, sizeof(src_code->bits));
        bit_inc(src_code->bits, MAX_BYTE_LEN);
    }
    return TOY_OK;
}

static int huffman_build_sym_code(huffman_code_t *codes, uint32_t len)
{
    /* 获取长度为N的字符的数量 */
    huffman_bl_t *bl_count = NULL;
    uint32_t bl_len;
    int ret = huffman_get_bl_counts(codes, len, &bl_count, &bl_len);
    if (ret != TOY_OK) {
        return ret;
    }

    /* 根据bl_count构建字符编码 */
    ret = huffman_build_sym_bits(bl_count, bl_len, codes, len);
    if (ret != TOY_OK) {
        free(bl_count);
        return ret;
    }

    free(bl_count);
    return TOY_OK;
}

static int huffman_build_code_table(huffman_node_t *root,
    huffman_code_t **codes, uint32_t *len)
{
    huffman_code_t *sym_codes = calloc(MAX_SYMBOL_NUM, sizeof(*sym_codes));
    if (sym_codes == NULL) {
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    /* 获取符号的编码长度 */
    int ret = huffman_get_sym_numbits(root, codes, len);
    if (ret != TOY_OK) {
        free(sym_codes);
        return ret;
    }

    /* 构建符号的编码 */
    ret = huffman_build_sym_code(sym_codes, MAX_SYMBOL_NUM);
    if (ret != TOY_OK) {
        free(sym_codes);
        return ret;
    }

    *len = MAX_SYMBOL_NUM;
    *codes = sym_codes;
    return TOY_OK;
}

// huffman 编码表写入缓存
static int huffman_write_code_table(huffman_code_t *codes, uint32_t codes_len,
    stream_t *out)
{
    int i = 0; 
    while (i < codes_len) {
        int j = i;
        // huffman编码长度采用游程编码
        while (j < codes_len && codes[j].numbits == codes[i].numbits) {
            j++;
        }
        if (out->pos >= out->size - 2) { // code编码最多占用2字节
            return TOY_ERR_HUFFMAN_OUTBUF_INSUFFICIET;
        }
        out->data[out->pos] = codes[i].numbits;
        if (j == i + 1) {
            out->data[out->pos] |= 0x80; // 1000 0000
            out->pos++;
        } else {
            out->data[out->pos] &= 0x7f; // 0111 1111
            out->pos++;
            out->data[out->pos] = j - i;
            out->pos++;
        }
        i = j;
    }
    return TOY_OK;
}

static int huffman_write_header(stream_t *in, stream_t *out)
{
    if (out->pos >= out->size - 4) { // code编码最多占用2字节
        return TOY_ERR_HUFFMAN_OUTBUF_INSUFFICIET;
    }

    uint32_t *cur_data = (uint32_t *)&out->data[out->pos]
    *cur_data = in->size;
    out->pos += sizeof(uint32_t);
    return TOY_OK;
}

static int huffman_write_syms(stream_t *in, stream_t *out,
    huffman_code_t *codes, uint32_t codes_len)
{
    for (uint32_t i = 0; i < in->size; i++) {
        
    }
}

static int huffman_write_data(stream_t *in, stream_t *out,
    huffman_code_t *codes, uint32_t codes_len)
{
    // 写编码头部
    int ret = huffman_write_header(in, out);
    if (ret != TOY_OK) {
        return ret;
    }

    // 写码表
    ret = huffman_write_code_table(codes, codes_len, out);
    if (ret != TOY_OK) {
        return ret;
    }

    // 写字符数据
    ret = huffman_write_syms(in, out, codes, codes_len);
    if (ret != TOY_OK) {
        return ret;
    }
    return TOY_OK;
}
int huffman_encode(stream_t *in, stream_t *out)
{
    if (in == NULL || out == NULL) {
        return TOY_ERR_HUFFMAN_INVALID_PARA;
    }

    huffman_node_t *sym_nodes[MAX_SYMBOL_NUM] = {0};
    /* 计算字符频率 */
    int ret = huffman_calc_symbol_frequencies(sym_nodes, MAX_SYMBOL_NUM, in->data, in->size);
    if (ret != TOY_OK) {
        huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
        return ret;
    }

    /* 构建霍夫曼树 */
    ret = huffman_build_tree(sym_nodes, MAX_SYMBOL_NUM);
    if (ret != TOY_OK) {
        huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
        return ret;
    }

    /* 构建编码表 */
    huffman_code_t *codes = NULL;
    uint32_t codes_len;
    ret = huffman_build_code_table(sym_nodes[0], &codes, &codes_len);
    if (ret != TOY_OK) {
        huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
        return ret;
    }

    /* 写入编码表*/
    out->size = in->size;
    out->data = malloc(out->size);
    if (out->data == NULL) {
        huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
        free(codes);
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }
    /* 字符使用霍夫曼编码并写入 */


    huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
    free(codes);

    return TOY_OK;
}

int huffman_free_stream(stream_t *strm)
{
    if (strm->data != NULL) {
        free(strm->data);
        strm->data = NULL;
        strm->size = 0;
        strm->pos = 0;
    }
}