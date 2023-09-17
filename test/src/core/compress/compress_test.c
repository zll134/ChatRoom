/* ********************************
 * Author:       Zhanglele
 * Description:  红黑树功能测试模块
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

// 匹配串较段的字符串
static const char *g_short_match_str = "Elephants Elephants.";

// 匹配串较长的字符串
static const char *g_long_match_str ="AAAaaaBBBbbbCCCcccDDDdddEEEeeeFFFfffGGG"
"gggHHHhhhIIIiiiJJJjjjKKKkkkLLLlllMMMmmmNNNnnnOOOoooPPPpppQQQqqqRRRrrrSSSsssTTTttt  "
"AAAaaaBBBbbbCCCcccDDDdddEEEeeeFFFfffGGGgggHHHhhhIIIiiiJJJjjjKKKkkkLLLlllMMMmmmNNN"
"nnnOOOoooPPPpppQQQqqqRRRrrrSSSsssTTTttt";

// 长字符串
static const char *g_long_str = "The Wood River Branch Railroad was a shortline railroad "
"in Rhode Island, United States. Chartered in 1872 and opened on July 1, 1874, the 5.6-mile "
"(9.0 km) line connected Hope Valley, Rhode Island, to the New York, Providence and Boston "
"Railroad (known as the Stonington Line) mainline at Wood River Junction. Though always "
"nominally independent, the company was closely affiliated with the Stonington Line and "
"its successor, the New York, New Haven and Hartford Railroad (the New Haven), which held "
"significant portions of its stock. \n"
"The Wood River Branch carried both passengers and freight for local mills and other industries. "
"A small operation, the company owned only one or two locomotives at any given time. Rhode Island "
"citizen Ralph C. Watrous became president of the railroad in 1904, and remained involved in its "
"operation for the next 33 years. He defended the railroad from several attempts at abandonment. "
"A major flood in November 1927 severed the line and suspended all operations. The company "
"considered abandonment, but ultimately local citizens and the New Haven agreed to rebuild the "
"damaged segments and return the line to service for freight only, using a gasoline locomotive.\n"
"Abandonment was considered again in 1937, but the New Haven instead agreed to sell the line for "
"$301 to businessman Roy Rawlings, owner of a grain mill that was the line's biggest customer. "
"He ran the company with his family and a small staff until 1947. That year, both his mill and "
"two other Hope Valley industries were destroyed by fire. Lacking enough business to justify "
"operating expenses, the railroad ceased operations and was abandoned in its entirety in August "
"1947. Little of the line remains as of 2018.\n";

// 多匹配串的字符串
static const char *g_multi_match_str = "Elep Elep-Elep:Ele,Elep.Elep;Elep-Elep=Elep";

static lz_compressor_t *g_comp = NULL;

TEST_SETUP(compress_test)
{
    lz_option_t option = {
        .level = LZ_MAX_COMPRESS_LEVEL
    };
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
 *  用例1: 字符串匹配块长度小于31长度。压缩：21 -> 17
 */
TEST(compress_test, Compress_and_decompress_When_match_is_short)
{
    uint8_t *input = (uint8_t *)g_short_match_str;
    uint32_t input_len = strlen(g_short_match_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

/**
 *  用例2: 字符串匹配块长度大于31长度。压缩： 243 -> 129
 */
TEST(compress_test, Compress_and_decompress_When_match_is_long)
{
    uint8_t *input = (uint8_t *)g_long_match_str;
    uint32_t input_len = strlen(g_long_match_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

/**
 *  用例3: 长字符串匹配块压缩。压缩：1717 -> 1517
 */
TEST(compress_test, Compress_and_decompress_When_string_is_long)
{
    uint8_t *input = (uint8_t *)g_long_str;
    uint32_t input_len = strlen(g_long_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

/**
 *  用例4: 多匹配块压缩。压缩：1717 -> 1517
 */
TEST(compress_test, Compress_and_decompress_When_multi_match)
{
    uint8_t *input = (uint8_t *)g_multi_match_str;
    uint32_t input_len = strlen(g_multi_match_str) + 1;

    test_assert_compress_and_decompress(input, input_len);
}

TEST_SUITE_RUNNER(compress_test)
{
    //RUN_TEST_CASE(compress_test, Compress_and_decompress_When_match_is_short);
    //RUN_TEST_CASE(compress_test, Compress_and_decompress_When_match_is_long);
    //RUN_TEST_CASE(compress_test, Compress_and_decompress_When_string_is_long);
    RUN_TEST_CASE(compress_test, Compress_and_decompress_When_multi_match);
}

int main()
{
    RUN_TEST_SUITE(compress_test);
    return 0;
}