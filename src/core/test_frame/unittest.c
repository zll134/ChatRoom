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
#include <string.h>

typedef struct {
    bool case_passed;
    uint32_t passed_num;
    uint32_t failed_num;
    unit_test_suite_t test_suite;
    unit_test_case_t test_case;
} unit_test_ctx_t;

unit_test_ctx_t g_running_ctx = {0};

void unit_print(uint32_t font_color, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);

    printf("\033[40;%dm%s\033[0m", font_color, buf);

    va_end(ap);
}

void unit_test_suite_runner(unit_test_suite_t *suite)
{
    unit_print(FONT_GREEN, "\n[==========]");
    unit_print(FONT_WHITE, " Run test suite %s\n", suite->group);

    (void)memcpy(&g_running_ctx.test_suite, suite, sizeof(*suite));

    if (suite->setup) {
        suite->setup();
    }

    suite->test_body();

    if (suite->tear_down) {
        suite->tear_down();
    }

    unit_print(FONT_GREEN, "[==========]");
    unit_print(FONT_WHITE, " %u test passed, %u test failed in suite %s\n",
        g_running_ctx.passed_num, g_running_ctx.failed_num, suite->group);
}

static void unit_test_case_setup(unit_test_case_t *test_case)
{
    unit_print(FONT_GREEN, "[-RUN -----]");
    unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->name);

    g_running_ctx.case_passed = true;
    (void)memcpy(&g_running_ctx.test_case, test_case, sizeof(*test_case));

    if (test_case->setup) {
        test_case->setup();
    }
}

static void unit_test_case_tear_down(unit_test_case_t *test_case)
{
    if (g_running_ctx.case_passed) {
        g_running_ctx.passed_num++;
        unit_print(FONT_GREEN, "[-------OK-]");
        unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->name);
    } else {
        g_running_ctx.failed_num++;
        unit_print(FONT_RED, "[--FAILED--]");
        unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->name);
    }

    if (test_case->tear_down) {
        test_case->tear_down();
    }
}

void unit_test_case_runner(unit_test_case_t *test_case)
{
    /* 执行测试用例准备动作 */
    unit_test_case_setup(test_case);

    /* 执行测试用例 */
    test_case->test_body();

    /* 执行测试用例销毁动作 */
    unit_test_case_tear_down(test_case);
}

static void unit_test_failed_proc(uint32_t assert_type)
{
    g_running_ctx.case_passed = false;
    if (assert_type == ASSERTION_TYPE_ASSERT) {
        unit_test_case_tear_down(&g_running_ctx.test_case);
        g_running_ctx.test_suite.tear_down();
        exit(0);
    }
}

void unit_test_assert_boolean(bool condition, bool expected, uint32_t assert_type, 
    const char *file, uint32_t line)
{
    if (condition == expected) {
        return;
    }

    unit_print(FONT_WHITE, "%s(line %u): assertion failed:\nactual %u\nexpeced %u\n",
        file, line, condition, expected);

    unit_test_failed_proc(assert_type);
}

void unit_test_assert_integer(uint32_t actual, uint32_t expected, uint32_t assert_type, 
    const char *file, uint32_t line)
{
    if (actual == expected) {
        return;
    }

    unit_print(FONT_WHITE, "%s(line %u): assertion failed:\nactual %u\nexpeced %u\n",
        file, line, actual, expected);

    unit_test_failed_proc(assert_type);
}