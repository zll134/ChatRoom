/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框架
 * create time: 2022.4.17
 ********************************/
#include "unittest_inner.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "unittest_print.h"

unit_test_ctx_t g_running_ctx = {0};

unit_test_ctx_t *unit_get_ctx()
{
    return &g_running_ctx;
}

void unit_test_finish()
{
    exit(0);
}

static unit_test_suite_t *unit_test_find_suite(unit_test_ctx_t *ctx,
    const char *group)
{
    if (ctx->test_suite == NULL) {
        return NULL;
    }

    unit_test_suite_t *cur = ctx->test_suite;
    while (cur != NULL) {
        if (strcmp(cur->group, group) == 0) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

void unit_test_add_suite(unit_test_suite_t *suite)
{
    unit_test_ctx_t *ctx = &g_running_ctx;
    unit_test_suite_t *found = unit_test_find_suite(ctx, suite->group);
    if (found != NULL) {
        unit_print(FONT_RED, "Test suite existed, group %s.\n", suite->group);
        return;
    }

    unit_test_suite_t *test_suite = calloc(1, sizeof(*test_suite));
    if (test_suite == NULL) {
        unit_print(FONT_RED, "Calloc for suite failed, group:%s.\n", suite->group);
        return;
    }

    memcpy(test_suite, suite, sizeof(*suite));
    unit_test_suite_t *root = ctx->test_suite;
    if (root == NULL) {
        ctx->test_suite = test_suite;
        return;
    }

    ctx->test_suite = test_suite;
    test_suite->next = root;
}

static int unit_test_case_compare(unit_test_case_t *a, unit_test_case_t *b)
{
    int ret = strcmp(a->file_name, b->file_name);
    if (ret != 0) {
        return ret;
    }

    return a->line_num < b->line_num ? -1 : 1;
}

void unit_test_add_case(unit_test_case_t *tcase)
{
    unit_test_ctx_t *ctx = &g_running_ctx;
    unit_test_suite_t *found = unit_test_find_suite(ctx, tcase->group);
    if (found == NULL) {
        unit_print(FONT_RED, "Test suite not exist, group:%s.\n", tcase->group);
        return;
    }

    unit_test_case_t *test_case = calloc(1, sizeof(*test_case));
    if (test_case == NULL) {
        unit_print(FONT_RED, "Calloc for add failed, case %s.%s.\n",
            tcase->group, tcase->case_name);
        return;
    }

    (void)memcpy(test_case, tcase, sizeof(*tcase));

    if (found->test_case == NULL) {
        found->test_case = test_case;
        return;
    }

    unit_test_case_t *cur = found->test_case;
    while (cur->next != NULL) {
        if ((unit_test_case_compare(cur, test_case) == -1) &&
            (unit_test_case_compare(test_case, cur->next) == -1)) {
            test_case->next = cur->next;
            cur->next = test_case;
            return;
        }
        cur = cur->next;
    }
    cur->next = test_case;
    test_case->next = NULL;
}

static void unit_test_case_setup(unit_test_suite_t *suite,
    unit_test_case_t *test_case)
{
    unit_print(FONT_GREEN, "[-RUN -----]");
    unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->case_name);

    g_running_ctx.case_passed = true;

    if (suite->case_setup) {
        suite->case_setup();
    }
}

static void unit_test_case_tear_down(unit_test_suite_t *suite,
    unit_test_case_t *test_case)
{
    if (g_running_ctx.case_passed) {
        g_running_ctx.passed_num++;
        unit_print(FONT_GREEN, "[-------OK-]");
        unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->case_name);
    } else {
        g_running_ctx.failed_num++;
        unit_print(FONT_RED, "[--FAILED--]");
        unit_print(FONT_WHITE, " %s.%s\n", test_case->group, test_case->case_name);
    }

    if (suite->case_teardown) {
        suite->case_teardown();
    }
}

static void unit_test_case_runner(unit_test_suite_t *suite, unit_test_case_t *test_case)
{
    /* 执行测试用例准备动作 */
    unit_test_case_setup(suite, test_case);

    /* 执行测试用例 */
    test_case->test_body();

    /* 执行测试用例销毁动作 */
    unit_test_case_tear_down(suite, test_case);
}

static void unit_test_suite_runner(unit_test_suite_t *suite)
{
    unit_print(FONT_GREEN, "\n[==========]");
    unit_print(FONT_WHITE, " Run test suite: %s\n", suite->group);

    g_running_ctx.cur_suite = suite;

    if (suite->suite_setup) {
        suite->suite_setup();
    }

    unit_test_case_t *cur_case = suite->test_case;
    while (cur_case != NULL) {
        g_running_ctx.cur_case = cur_case;
        unit_test_case_runner(suite, cur_case);
        cur_case = cur_case->next;
    }

    if (suite->suite_teardown) {
        suite->suite_teardown();
    }

    unit_print(FONT_GREEN, "[==========]");
    unit_print(FONT_WHITE, " %u test passed, %u test failed in suite %s\n",
        g_running_ctx.passed_num, g_running_ctx.failed_num, suite->group);
}

int unit_test_run(int argc, char **argv)
{
    unit_test_suite_t *cur_suite = g_running_ctx.test_suite;
    while (cur_suite != NULL) {
        unit_test_suite_runner(cur_suite);
        cur_suite = cur_suite->next;
    }
    return 0;
}

