/* ********************************
 * Author:       Zhanglele
 * Description:  压缩算法功能测试模块
 * create time: 2024.02.29
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "log.h"
#include "unittest.h"
#include "pub_def.h"
#include "dict.h"
#include "compress.h"

/* 测试用字符串 */
#include "test_compress_string.c"
compressor_t *g_compressor = NULL;
TEST_SETUP(compress_test)
{
    compressor_option_t option = {
        .level = MAX_COMPRESS_LEVEL
    };
    g_compressor = compressor_create(&option);
    ASSERT_TRUE(g_compressor != NULL)
}

TEST_TEAR_DOWN(compress_test)
{
    compressor_destroy(g_compressor);
}

TEST_CASE_SETUP(compress_test)
{
}

TEST_CASE_TEAR_DOWN(compress_test)
{
}
static void test_compressor_encode_and_decode(uint8_t *input, uint32_t input_len)
{
    stream_t in = {0};
    in.data = input;
    in.size = input_len;

    /* 编码 */
    stream_t out = {0};
    int ret = compressor_encode(g_compressor, &in, &out);
    ASSERT_EQ(ret, TOY_OK);

    // 打印关键压缩信息
    TEST_INFO(" Origin data len %u, compressed data len %u",
        input_len, out.size);

    /* 解码 */
    stream_t rebuild = {0};
    ret = compressor_decode(g_compressor, &out, &rebuild);
    ASSERT_EQ(ret, TOY_OK);

    // 数据比较
    ASSERT_EQ(in.size, rebuild.size);
    for (int i = 0; i < in.size; i++) {
        ASSERT_EQ(in.data[i], rebuild.data[i]);
    }

    free(out.data);
    free(rebuild.data);
}

TEST(compress_test, Compress_and_decompress_When_match_is_short)
{
    uint8_t *in = (uint8_t *)g_long_match_str;
    uint32_t in_len = strlen(g_long_match_str) + 1;

    test_compressor_encode_and_decode(in, in_len);
}

/**
 *  用例3: 长字符串匹配块压缩。
 *      压缩优化：7966 -> 5402
 */
TEST(compress_test, Compress_and_decompress_When_string_is_long)
{
    uint8_t *input = (uint8_t *)g_long_str;
    uint32_t input_len = strlen(g_long_str) + 1;

    test_compressor_encode_and_decode(input, input_len);
}

int main(int argc, char **argv)
{
    TEST_MAIN(argc, argv);
    return 0;
}