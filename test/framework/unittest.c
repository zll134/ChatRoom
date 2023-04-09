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
    FONT_WHITE = 37,
};

typedef struct {
    bool case_passed;
} unit_test_ctx_t;

unit_test_ctx_t g_running_ctx = {0};

static void unit_print(uint32_t font_color, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);

    printf("\033[40;%dm%s\033[0m", font_color, buf);

    va_end(ap);
}

void unit_test_boolean(bool condition, bool expected, uint32_t assert_type, 
    const char *file, uint32_t line)
{
    if (condition == expected) {
        return;
    }
    g_running_ctx.case_passed = false;
    unit_print(FONT_WHITE, "%s(%u): assertion failed:\nactual %u\nexpeced %u\n",
        file, line, condition, expected);
}

void unit_test_case_runner(unit_test_case_t *test_case)
{
    unit_print(FONT_GREEN, "[ RUN      ]");
    unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->name);

    g_running_ctx.case_passed = true;
    if (test_case->setup) {
        test_case->setup();
    }
    
    test_case->test_body();

    if (test_case->tear_down) {
        test_case->tear_down();
    }

    if (g_running_ctx.case_passed) {
        unit_print(FONT_GREEN, "[       OK ]");
        unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->name);
    } else {
        unit_print(FONT_RED, "[  FAILED  ]");
        unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->name);
    }
}