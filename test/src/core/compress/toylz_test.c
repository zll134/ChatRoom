/* ********************************
 * Author:       Zhanglele
 * Description:  lz77算法功能测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "log.h"
#include "unittest.h"
#include "toylz.h"
#include "pub_def.h"
#include "dict.h"

/* 测试用字符串 */
#include "test_compress_string.c"

static lz_compressor_t *g_comp = NULL;

TEST_SETUP(toylz_test)
{
    lz_option_t option = {
        .level = LZ_MAX_COMPRESS_LEVEL
    };
    g_comp = lz_create_compressor(&option);
    ASSERT_TRUE(g_comp != NULL);
}

TEST_TEAR_DOWN(toylz_test)
{
    lz_destroy_compressor(g_comp);
}

TEST_CASE_SETUP(toylz_test)
{
}

TEST_CASE_TEAR_DOWN(toylz_test)
{
}

static void test_assert_compress_and_decompress(uint8_t *input, uint32_t input_len)
{
    uint8_t *out = malloc(input_len);
    uint8_t *decomp_out = malloc(input_len);
    ASSERT_TRUE(out != NULL);
    ASSERT_TRUE(decomp_out != NULL);

    // 数据压缩
    lz_stream_t strm = {0};
    strm.in = (uint8_t *)input;
    strm.in_size = input_len;
    strm.out = out;
    strm.out_size = strm.in_size ;
    int ret = lz_compress(g_comp, &strm);
    ASSERT_EQ(ret, TOY_OK);
    ASSERT_TRUE(strm.out_total < input_len);

    // 打印关键压缩信息
    TEST_INFO(" Origin data len %u, compressed data len %u",
        input_len, strm.out_total);

    // 数据解压
    lz_stream_t decom_strm = {0};
    decom_strm.in = out;
    decom_strm.in_size = strm.out_total;
    decom_strm.out = decomp_out;
    decom_strm.out_size = input_len;
    ret = lz_decompress(g_comp, &decom_strm);
    ASSERT_EQ(ret, TOY_OK);

    // 数据比较
    ASSERT_EQ(decom_strm.out_total, strm.in_size);
    for (int i = 0; i < strm.in_size; i++) {
        ASSERT_EQ(strm.in[i], decom_strm.out[i]);
    }

    // 内存释放
    free(out);
    free(decomp_out);
}

/**
 *  用例1: 字符串匹配块长度小于31长度。
 *      压缩优化：21 -> 17 => 21 -> 16
 */
TEST(toylz_test, Compress_and_decompress_When_match_is_short)
{
    uint8_t *input = (uint8_t *)g_short_match_str;
    uint32_t input_len = strlen(g_short_match_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

/**
 *  用例2: 字符串匹配块长度大于31长度。
 *      压缩优化： 243 -> 129
 */
TEST(toylz_test, Compress_and_decompress_When_match_is_long)
{
    uint8_t *input = (uint8_t *)g_long_match_str;
    uint32_t input_len = strlen(g_long_match_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

/**
 *  用例3: 长字符串匹配块压缩。
 *      压缩优化：7966 -> 6280 =>  7966 -> 5402
 */
TEST(toylz_test, Compress_and_decompress_When_string_is_long)
{
    uint8_t *input = (uint8_t *)g_long_str;
    uint32_t input_len = strlen(g_long_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

/**
 *  用例4: 多匹配块压缩。
 *      压缩优化：44 -> 40 => 44 -> 34
 */
TEST(toylz_test, Compress_and_decompress_When_multi_match)
{
    uint8_t *input = (uint8_t *)g_multi_match_str;
    uint32_t input_len = strlen(g_multi_match_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

int main(int argc, char **argv)
{
    TEST_MAIN(argc, argv);
    return 0;
}