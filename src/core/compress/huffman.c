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
#include "big_int.h"
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
    uint32_t count;                         /* 字符数统计 */
    bool is_leaf;                           /* 是否为叶节点 */
    union {
        struct {
            struct huffman_node_tag *zero;
            struct huffman_node_tag *one;
        };
        uint8_t symbol;                     /* 字符值 */
    };
    uint32_t depth;                        /* 从根节点开始的深度 */
} huffman_node_t;

typedef struct huffman_code_tag {
    uint32_t numbits;
    long_integer_t *bit_val;
    char bits_str[MAX_SYMBOL_NUM];
} huffman_code_t;

/* byte length */
typedef struct {
    uint32_t count;
    huffman_code_t start_code;
} huffman_bl_t;

/* huffman树 */
typedef struct {
    huffman_node_t *sym_nodes[MAX_SYMBOL_NUM]; /* 第一个元素是根节点 */
} huffman_tree_t;

/* huffman编码表 */
typedef struct {
    huffman_code_t sym_codes[MAX_SYMBOL_NUM]; /* 第一个元素是根节点 */
} huffman_code_table_t;

static huffman_node_t *huffman_create_sym_node(uint8_t symbol)
{
    huffman_node_t *node = calloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }
    node->is_leaf = true;
    node->parent = NULL;
    node->symbol = symbol;
    node->count = 0;
    node->depth = 0;
    return node;
}

static void huffman_free_sym_nodes(huffman_node_t *sym_node)
{
    if (sym_node == NULL) {
        return;
    }

    if (sym_node->is_leaf) {
        free(sym_node);
        return;
    }

    if (sym_node->zero != NULL) {
        huffman_free_sym_nodes(sym_node->zero);
        free(sym_node->zero);
        sym_node->zero = NULL;
    }

    if (sym_node->one != NULL) {
        huffman_free_sym_nodes(sym_node->one);
        free(sym_node->one);
        sym_node->one = NULL;
    }
}

/* 释放huffman树 */
static void huffman_free_tree(huffman_tree_t *tree)
{
    for (uint32_t i = 0; i < MAX_SYMBOL_NUM; i++) {
        /* 空指针不进行处理 */
        if (tree->sym_nodes[i] == NULL) {
            continue;
        }

        huffman_free_sym_nodes(tree->sym_nodes[i]);
        tree->sym_nodes[i] = NULL;
    }
}

static int huffman_calc_symbol_frequencies(const stream_t *in, huffman_tree_t *tree)
{
    for (uint32_t i = 0; i < in->size; i++) {
        uint8_t symbol = in->data[i];
        if (symbol >= MAX_SYMBOL_NUM) {
            huffman_free_tree(tree);
            return TOY_ERR_HUFFMAN_SYMBOL_OVERFLOW;
        }

        if (tree->sym_nodes[symbol] != NULL) {
            tree->sym_nodes[symbol]->count++;
            continue;
        }
        /* 节点不存在时则，创建一个 */
        tree->sym_nodes[symbol] = huffman_create_sym_node(symbol);
        if (tree->sym_nodes[symbol] == NULL) {
            huffman_free_tree(tree);
            return TOY_ERR_HUFFMAN_MALLOC_FAILED;
        }
    }

    return TOY_OK;
}

static int huffman_sym_node_compare(const void* p1, const void* p2)
{
    const huffman_node_t *node1 = *(const huffman_node_t**)p1;
    const huffman_node_t *node2 = *(const huffman_node_t**)p2;

    if (node1 == NULL && node2 == NULL) {
        return 0;
    }

    /* node2 排前面 */
    if (node1 == NULL) {
        return 1;
    }

     /* node1 排前面 */
    if (node2 == NULL) {
        return -1;
    }

    /* 频率低节点排在前面 */
    if (node1->count > node2->count) {
        return 1;
    } else if (node1->count < node2->count) {
        return -1;
    }

    return 0;
}

static huffman_node_t *huffman_node_merge(huffman_node_t *zero, huffman_node_t *one)
{
    huffman_node_t *node = calloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->is_leaf = false;
    node->parent = NULL;
    node->zero = zero;
    node->one = one;
    node->count = zero->count + one->count;

    /* 设置父节点 */
    zero->parent = node;
    one->parent = node;

    return node;
}

/* 构建霍夫曼树 */
static int huffman_build_sym_tree(const stream_t *in, huffman_tree_t *tree)
{
    huffman_node_t **sym_nodes = tree->sym_nodes;
    /* 获取符号的个数 */
    uint32_t total_count = 0;
	for (int i = 0; i < MAX_SYMBOL_NUM; i++) {
        if (sym_nodes[i] != NULL) {
            total_count++;
        }
    }

    for (int count = total_count; count >= 1; count--) {
        /* 按照频率排序 */
        qsort(sym_nodes, MAX_SYMBOL_NUM, sizeof(sym_nodes[0]), huffman_sym_node_compare);
        if (count <= 1) {
            break;
        }

        huffman_node_t *m1 = sym_nodes[0];
        huffman_node_t *m2 = sym_nodes[1];

        /* 将m1和m2合并成新的节点 */
        huffman_node_t *node = huffman_node_merge(m1, m2);
        if (node == NULL) {
            return TOY_ERR_HUFFMAN_NEW_NOSYM_NODE_FAILD;
        }

        sym_nodes[0] = node;
        sym_nodes[1] = NULL;
	}
    return TOY_OK;
}

static int huffman_get_sym_code_numbits(huffman_node_t *root,
    huffman_code_t *codes, uint32_t len)
{
    if (root == NULL) {
        return TOY_OK;
    }

    if (root->parent != NULL) {
        root->depth = root->parent->depth + 1;
    }

    /* 符号叶节点处理 */
    if (root->is_leaf) {
        if (root->symbol >= len) {
            return TOY_ERR_HUFFMAN_SYMBOL_OVERFLOW;
        }
        codes[root->symbol].numbits = root->depth;
        return TOY_OK;
    }

    return (huffman_get_sym_code_numbits(root->zero, codes, len) ||
        huffman_get_sym_code_numbits(root->one, codes, len));
}

static void huffman_bl_free(huffman_bl_t *bl, uint32_t len)
{
    if (bl == NULL) {
        return;
    }
    for (int i = 0; i < len; i++) {
        if (bl[i].start_code.bit_val != NULL) {
            integer_free(bl[i].start_code.bit_val);
        }
    }
    free(bl);
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
        if (codes[i].numbits > MAX_BITS_LEN) {
            return TOY_ERR_HUFFMAN_NUMBITS_OVERFLOW;
        }
        bl[codes[i].numbits].count++;
    }

    for (int i = 1; i < MAX_BITS_LEN + 1; i++) {
        bl[i].start_code.numbits = i;

        long_integer_t *bit_val = integer_new(0, MAX_BYTE_LEN);
        if (!bit_val) {
            huffman_bl_free(bl, MAX_BITS_LEN + 1);
            return TOY_ERR_HUFFMAN_NEW_BIG_INT;
        }

        bl[i].start_code.bit_val = bit_val;
    
        integer_copy(bl[i].start_code.bit_val, bl[i - 1].start_code.bit_val);

        integer_add(bl[i].start_code.bit_val, bl[i - 1].count);
        integer_shift_left(bl[i].start_code.bit_val, 1);
    }

    *bl_count = bl;
    *len = MAX_BITS_LEN + 1;
    return TOY_OK;
}

static void huffman_free_sym_codes_int(huffman_code_t *codes, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        if (codes[i].bit_val != NULL) {
            integer_free(codes[i].bit_val);
        }
    }
}

static int huffman_build_sym_bits(const huffman_bl_t *bl_counts, uint32_t bl_len,
    huffman_code_t *codes, uint32_t codes_len)
{
    for (int i = 0; i < codes_len; i++) {
        uint32_t numbits = codes[i].numbits;
        const huffman_code_t *start_code = &bl_counts[numbits].start_code;
        codes[i].bit_val = integer_new(0, MAX_BYTE_LEN);
        if (!codes[i].bit_val) {
            huffman_free_sym_codes_int(codes, codes_len);
            return TOY_ERR_HUFFMAN_NEW_BIG_INT;
        }

        integer_copy(codes[i].bit_val, start_code->bit_val);
        integer_inc(start_code->bit_val);
    }

    return TOY_OK;
}

static int huffman_build_code_str(huffman_code_t *codes, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        huffman_code_t *code = &codes[i];
        uint32_t numbits = code->numbits;
        while (numbits > 0) {
            uint8_t bit = integer_get_bit(code->bit_val, 0);
            code->bits_str[numbits - 1] = bit == 0 ? '0' : '1';
            integer_shift_right(code->bit_val, 1);
            numbits--;
        }
    }
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
    huffman_bl_free(bl_count, bl_len);
    if (ret != TOY_OK) {
        return ret;
    }

    /* 转换为字符串 */
    ret = huffman_build_code_str(codes, len);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

static int huffman_build_tree(const stream_t *in, huffman_tree_t *tree)
{
    /* 计算字符频率 */
    int ret = huffman_calc_symbol_frequencies(in, tree);
    if (ret != TOY_OK) {
        return ret;
    }

    /* 根据字符频率构建霍夫曼树 */
    ret = huffman_build_sym_tree(in, tree);
    if (ret != TOY_OK) {
        huffman_free_tree(tree);
        return ret;
    }

    return TOY_OK;
}

static int huffman_build_code_table(stream_t *in,
    huffman_code_table_t *codes)
{
    /* 构建霍夫曼树 */
    huffman_tree_t tree = {0};
    int ret = huffman_build_tree(in, &tree);
    if (ret != TOY_OK) {
        return ret;
    }

    /* 获取符号的huffman编码长度 */
    ret = huffman_get_sym_code_numbits(tree.sym_nodes[0], codes->sym_codes, MAX_SYMBOL_NUM);
    huffman_free_tree(&tree);
    if (ret != TOY_OK) {
        return ret;
    }

    /* 根据编码长度计算符号的编码 */
    ret = huffman_build_sym_code(codes->sym_codes, MAX_SYMBOL_NUM);
    if (ret != TOY_OK) {
        return ret;
    }

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

    uint32_t *cur_data = (uint32_t *)&out->data[out->pos];
    *cur_data = in->size;
    out->pos += sizeof(uint32_t);
    return TOY_OK;
}

static int huffman_write_syms(stream_t *in, stream_t *out,
    huffman_code_table_t *codes)
{
    for (uint32_t i = 0; i < in->size; i++) {
        uint8_t sym = in->data[i];
        huffman_code_t *code = &codes->sym_codes[sym];
        for (int j = 0; j < code->numbits; j++) {
            int ret = stream_write_bit(out, code->bits_str[j] - '0');
            if (ret != TOY_OK) {
                return ret;
            }
        }
    }

    return TOY_OK;
}

static int huffman_write_data(stream_t *in, stream_t *out,
    huffman_code_table_t *codes)
{
    // 写编码头部
    int ret = huffman_write_header(in, out);
    if (ret != TOY_OK) {
        return ret;
    }

    // 写码表
    ret = huffman_write_code_table(codes->sym_codes, MAX_SYMBOL_NUM, out);
    if (ret != TOY_OK) {
        return ret;
    }

    // 写字符数据
    ret = huffman_write_syms(in, out, codes);
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

    huffman_code_table_t *codes = calloc(1, sizeof(*codes));
    if (codes == NULL) {
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    /* 构建huffman编码表 */
    int ret = huffman_build_code_table(in, codes);
    if (ret != TOY_OK) {
        free(codes);
        return ret;
    }

    /* 写入编码表*/
    out->size = in->size;
    out->data = calloc(1, out->size);
    if (out->data == NULL) {
        free(codes);
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    /* 字符使用霍夫曼编码并写入 */
    ret = huffman_write_data(in, out, codes);
    free(codes);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

static int huffman_read_header(stream_t *in, uint32_t *data_size)
{
    *data_size = *(uint32_t *)in->data;
    in->pos += sizeof(uint32_t);
    return TOY_OK;
}

static int huffman_rebuild_numbits(stream_t *in, huffman_code_table_t *codes)
{
    int i = 0;
    while (i < MAX_SYMBOL_NUM) {
        uint8_t data = in->data[in->pos];
        in->pos++;

        uint8_t numbits = (data & 0x7f);
        uint8_t sym_num = 1;
        if ((data & 0x80) == 0) { // 1000 0000
            sym_num = in->data[in->pos];
            in->pos++;
        }

        for (int j = 0; j < sym_num; j++) {
            codes->sym_codes[i].numbits = numbits;
            i++;
        }
    }
    return TOY_OK;
}

static int huffman_add_symbits_to_tree(huffman_code_t *code, uint8_t sym,
    huffman_tree_t *tree)
{
    huffman_node_t *cur_node = tree->sym_nodes[0];
    int j = 0;
    while (j < code->numbits) {
        char bit = code->bits_str[j];

        if (bit = '0') {
            if (cur_node->zero == NULL) {
                cur_node->zero = calloc(1, sizeof(huffman_code_t));
                if (cur_node->zero != NULL) {
                    return TOY_ERR_HUFFMAN_MALLOC_FAILED;
                }
            }
            cur_node = cur_node->zero;
        } else {
            if (cur_node->one == NULL) {
                cur_node->one = calloc(1, sizeof(huffman_code_t));
                if (cur_node->one != NULL) {
                    return TOY_ERR_HUFFMAN_MALLOC_FAILED;
                }
            }
            cur_node = cur_node->one;
        }

        if (j == code->numbits - 1) {
            cur_node->is_leaf = true;
            cur_node->symbol = sym;
        } else {
            cur_node->is_leaf = false;
        }
        j++;
    }
    return TOY_OK;
}

static int huffman_rebuild_sym_tree(huffman_code_t *codes, uint32_t len,
    huffman_tree_t *tree)
{
    tree->sym_nodes[0] = calloc(1, sizeof(huffman_code_t));
    if (tree->sym_nodes[0] == NULL) {
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    for (int i = 0; i < len; i++) {
        huffman_code_t *code = &codes[i];
        
        int ret = huffman_add_symbits_to_tree(code, i, tree);
        if (ret != TOY_OK) {
            huffman_free_tree(tree);
            return ret;
        } 
    }

    return TOY_OK;
}


static int huffman_rebuild_tree(stream_t *in, huffman_tree_t *tree)
{
    huffman_code_table_t *codes = calloc(1, sizeof(*codes));
    if (codes == NULL) {
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    int ret = huffman_rebuild_numbits(in, codes);
    if (ret != TOY_OK) {
        free(codes);
        return ret;
    }

    ret = huffman_build_sym_code(codes->sym_codes, MAX_SYMBOL_NUM);
    if (ret != TOY_OK) {
        free(codes);
        return ret;
    }

    ret = huffman_rebuild_sym_tree(codes->sym_codes, MAX_SYMBOL_NUM, tree);
    if (ret != TOY_OK) {
        free(codes);
        return ret;
    }

    free(codes);
    return TOY_OK;
}

static int huffman_read_data(stream_t *in, stream_t *out, huffman_tree_t *tree)
{ 
    while (out->pos < out->size) {
        huffman_node_t *cur_node = tree->sym_nodes[0];

        while (!cur_node->is_leaf) {
            int bit = stream_read_bit(in);
            cur_node = bit == 0 ? cur_node->zero : cur_node->one;
        }

        out->data[out->pos] = cur_node->symbol;
        out->pos++;
    }

    return TOY_OK;
}

int huffman_decode(stream_t *in, stream_t *out)
{
    if (in == NULL || out == NULL) {
        return TOY_ERR_HUFFMAN_INVALID_PARA;
    }

    uint32_t data_size;
    huffman_read_header(in, &data_size);

    huffman_tree_t tree = {0};
    int ret = huffman_rebuild_tree(in, &tree);
    if (ret != TOY_OK) {
        return ret;
    }

    out->data = calloc(1, data_size);
    if (out->data != TOY_OK) {
        return TOY_ERR_HUFFMAN_MALLOC_FAILED;
    }

    out->size = data_size;

    ret = huffman_read_data(in, out, &tree);
    if (ret != TOY_OK) {
        return ret;
    }
    return TOY_OK;
}