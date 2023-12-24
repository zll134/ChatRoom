/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框断言模块
 * create time: 2023.12.17
 ********************************/
#include "unittest_assertion.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "unittest_inner.h"
#include "unittest_print.h"

static void unit_assert_failed_proc(uint32_t assert_type)
{
    unit_test_ctx_t *ctx = unit_get_ctx();
    ctx->case_passed = false;
    if (assert_type == ASSERTION_TYPE_ASSERT) {
        if (ctx->cur_suite->case_teardown) {
            ctx->cur_suite->case_teardown();
        }
        if (ctx->cur_suite->suite_teardown) {
            ctx->cur_suite->suite_teardown();
        }

        unit_test_finish();
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

    unit_assert_failed_proc(assert_type);
}

void unit_test_assert_integer(uint32_t actual, uint32_t expected, uint32_t assert_type, 
    const char *file, uint32_t line)
{
    if (actual == expected) {
        return;
    }

    unit_print(FONT_WHITE, "%s(line %u): assertion failed:\nactual %u\nexpeced %u\n",
        file, line, actual, expected);

    unit_assert_failed_proc(assert_type);
}