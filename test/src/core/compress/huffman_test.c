/* ********************************
 * Author:       Zhanglele
 * Description:  huffman模块测试
 * create time: 2023.12.18
 ********************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "unittest.h"

#include "huffman.h"

/* 测试用字符串 */
#include "test_compress_string.c"

TEST_SETUP(huffman_test)
{
}

TEST_TEAR_DOWN(huffman_test)
{
}

TEST_CASE_SETUP(huffman_test)
{
}

TEST_CASE_TEAR_DOWN(huffman_test)
{
}

static void test_huffman_encode_and_decode(uint8_t *input, uint32_t input_len)
{
    stream_t in = {0};
    in.data = input;
    in.size = input_len;

    /* 编码 */
    stream_t out = {0};
    int ret = huffman_encode(&in, &out);
    ASSERT_EQ(ret, TOY_OK);

    // 打印关键压缩信息
    TEST_INFO(" Origin data len %u, compressed data len %u",
        input_len, out.size);

    /* 解码 */
    stream_t rebuild = {0};
    ret = huffman_decode(&out, &rebuild);
    ASSERT_EQ(ret, TOY_OK);

    // 数据比较
    ASSERT_EQ(in.size, rebuild.size);
    for (int i = 0; i < in.size; i++) {
        ASSERT_EQ(in.data[i], rebuild.data[i]);
    }

    free(out.data);
    free(rebuild.data);
}

/**
 *  用例1: 小长度的字符串huffman压缩
 *      压缩优化：21 -> 17 => 21 -> 16
 */
TEST(huffman_test, test_short_match_str)
{
    uint8_t *input = (uint8_t *)g_short_match_str;
    uint32_t input_len = strlen(g_short_match_str) + 1;

    test_huffman_encode_and_decode(input, input_len);
}

/**
 *  用例2: 大长度的字符串huffman压缩
 *      压缩优化： 243 -> 188
 */
TEST(huffman_test, test_long_match_str)
{
    uint8_t *in = (uint8_t *)g_long_match_str;
    uint32_t in_len = strlen(g_long_match_str) + 1;
    test_huffman_encode_and_decode(in, in_len);
}

/**
 *  用例3: 真实字符串huffman压缩。
 *      压缩优化： 7966 -> 4768
 */
TEST(huffman_test, test_real_str)
{
    uint8_t *input = (uint8_t *)g_long_str;
    uint32_t input_len = strlen(g_long_str) + 1;

    test_huffman_encode_and_decode(input, input_len);
}

int main(int argc, char **argv)
{
    TEST_MAIN(argc, argv);
    return 0;
}