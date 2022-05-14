/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框架
 * create time: 2022.4.17
 ********************************/
#include "unittest.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

enum {
    FONT_RED = 31,
    FONT_GREEN = 32,
};
static void print(uint32_t font_color, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);

    printf("\033[40;%dm%s \033[0m", font_color, buf);

    va_end(ap);
}

static bool test_passed;

void assert_ne(uint64_t left, uint64_t right)
{
    if (left == right) {
        test_passed = false;
        exit(-1);
    }
}
void assert_eq(uint64_t left, uint64_t right)
{
    if (left != right) {
        test_passed = false;
        exit(-1);
    }
}

void run_tests(unit_test_t *tests, int len)
{
    int total_passed = 0;
    for (int i =0; i < len; i++) {
        test_passed = true;
        if (tests[i].func_type == UNIT_TEST_TYPE_RUN) {
            print(FONT_GREEN, "[test case]: %s ", tests[i].name);
            tests[i].func();
            if (test_passed) {
                print(FONT_GREEN, "pass\n");
                total_passed++;
            }
        }
    }

    print(FONT_GREEN, "total %d tests, passed %d tests\n", len, total_passed);
}