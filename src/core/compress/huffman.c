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

/*
1. huffman 压缩格式
    格式：
        码表 + 字符编码
    码表的格式参考rfc1951，存储所有字符的长度的游程编码。
*/

#define MAX_SYMBOL_NUM 256

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
    uint32_t bits;
} huffman_code_t;

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

static int huffman_build_code_table(huffman_node_t *root,
    huffman_code_t *codes, uint32_t len)
{
    /* 获取符号的编码长度 */
    int ret = huffman_get_sym_numbits(root, codes, len);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

int huffman_encode(uint8_t *in, uint32_t in_len, uint8_t **out, uint32_t *out_len)
{
    if (out == NULL || out_len == NULL) {
        return TOY_ERR_HUFFMAN_INVALID_PARA;
    }

    huffman_node_t *sym_nodes[MAX_SYMBOL_NUM] = {0};

    /* 计算字符频率 */
    int ret = huffman_calc_symbol_frequencies(sym_nodes, MAX_SYMBOL_NUM, in, in_len);
    if (ret != TOY_OK) {
        huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
        return ret;
    }

    /* 构建霍夫曼树 */
    int ret = huffman_build_tree(sym_nodes, MAX_SYMBOL_NUM);
    if (ret != TOY_OK) {
        huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
        return ret;
    }

    /* 构建编码表 */
    huffman_code_t codes[MAX_SYMBOL_NUM] = {0};
    int ret = huffman_build_code_table(sym_nodes[0], codes, MAX_SYMBOL_NUM);
    if (ret != TOY_OK) {
        return ret;
    }

    /* 字符使用霍夫曼编码 */
    huffman_free_sym_nodes(sym_nodes, MAX_SYMBOL_NUM);
    return TOY_OK;
}