/* ********************************
 * Author:       Zhanglele
 * Description:  压缩算法
 * create time:  2024.02.25
 ********************************/
#include "compress.h"

#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "huffman.h"
compressor_t *compressor_create(compressor_option_t *option)
{
    if (option->level > MAX_COMPRESS_LEVEL ||
        option->level < MIN_COMPRESS_LEVEL) {
        ERROR("[compress] Compress level is invalid.");
        return NULL;
    }

    compressor_t *comp = calloc(1, sizeof(*comp));
    if (!comp) {
        ERROR("[compress] Calloc for compressor failed.");
        return NULL;
    }

    lz_option_t lz_option = {
        .level = option->level
    };
    comp->lz_compressor = lz_create_compressor(&lz_option);
    if (comp->lz_compressor == NULL) {
        free(comp);
        return NULL;
    }

    return comp;
}

void compressor_destroy(compressor_t *compressor)
{
    if (compressor == NULL) {
        return;
    }
    free(compressor->lz_compressor);
    free(compressor);
}

int compressor_encode(compressor_t *compressor, stream_t *in, stream_t *out)
{
    uint8_t *out_data = malloc(in->size);
    if (out == NULL) {
        return TOY_ERR;
    }

    // 数据压缩
    lz_stream_t strm = {0};
    strm.in = (uint8_t *)in->data;
    strm.in_size = in->size;
    strm.out = out_data;
    strm.out_size = in->size ;
    int ret = lz_compress(compressor->lz_compressor, &strm);
    if (ret != TOY_OK) {
        free(out);
        return ret;
    }

    stream_t in_strm = {0};
    in_strm.data = out_data;
    in_strm.size = strm.out_total;

    /* 编码 */
    ret = huffman_encode(&in_strm, out);
    free(out_data);
    if (ret != TOY_OK) {
        return ret;
    }

    return TOY_OK;
}

int compressor_decode(compressor_t *compressor, stream_t *in, stream_t *out)
{
    stream_t huffman_in = {0};
    huffman_in.data = in->data;
    huffman_in.size = in->size;

    stream_t huffman_out = {0};
    /* 编码 */
    int ret = huffman_decode(&huffman_in, &huffman_out);
    if (ret != TOY_OK) {
        return ret;
    }

    out->data = calloc(1, MAX_DECOMPRESS_LEN);
    if (out->data == NULL) {
        return TOY_ERR;
    }
    out->capacity = MAX_DECOMPRESS_LEN;

    lz_stream_t decom_strm = {0};
    decom_strm.in = huffman_out.data;
    decom_strm.in_size = huffman_out.size;
    decom_strm.out = out->data;
    decom_strm.out_size = out->capacity;
    ret = lz_decompress(compressor->lz_compressor, &decom_strm);
    free(huffman_out.data);
    if (ret != TOY_OK) {
        return ret;
    }

    out->size = decom_strm.out_total;
    return TOY_OK;
}