/* ********************************
 * Author:       Zhanglele
 * Description:  红黑树功能测试模块
 * create time: 2022.01.23
 ********************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "unittest.h"
#include "toylz.h"

#include "common_data.c"

lz_compressor_t *g_comp = NULL;
TEST_SETUP(compress_test)
{
    lz_option_t option = {
        .level = LZ_MAX_COMPRESS_LEVEL;
    }
    g_comp = lz_create_compressor(&option);
    ASSERT_TRUE(g_comp != NULL);
}

TEST_TEAR_DOWN(compress_test)
{
    lz_destroy_compressor(g_comp);
}

TEST_CASE_SETUP(compress_test)
{
}

TEST_CASE_TEAR_DOWN(compress_test)
{
}

TEST(compress_test, test_compress_and_decompress)
{
    uint32_t *values = g_test_data1;
    uint32_t num = ARRAY_SIZE(g_test_data1);
    uint8_t *out = malloc(num * sizeof(uint32_t));
    ASSERT_TRUE(out != NULL);
    uint8_t *decomp_out = malloc(num * sizeof(uint32_t));
    ASSERT_TRUE(decomp_out != NULL);

    // 数据压缩
    lz_stream_t strm = {0};
    strm.in = values;
    strm.in_size = num * sizeof(uint32_t);
    strm.out = out;
    strm.out_size = strm.in_size ;
    int ret = lz_compress(g_comp, &strm);
    ASSERT_EQ(ret, TOY_OK);

    // 数据解压
    lz_stream_t decom_strm = {0};
    decom_strm.in = out;
    decom_strm.in_size = strm.out_total;
    decom_strm.out = decomp_out;
    decom_strm.out_size = num * sizeof(uint32_t);
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

TEST_SUITE_RUNNER(compress_test)
{
    RUN_TEST_CASE(compress_test, test_compress_and_decompress);
}

int main()
{
    RUN_TEST_SUITE(compress_test);
    return 0;
}