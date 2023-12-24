/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试框架内部模块
 * create time: 2023.12.17
 ********************************/
#ifndef UNITTEST_INNER_H
#define UNITTEST_INNER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_GROUP_LEN 64
#define MAX_TEST_NAME_LEN 128
#define MAX_FILE_NAME_LEN 64

typedef void (unit_func_t)(void);

typedef struct unit_test_case_tag {
    unit_func_t *test_body;
    char group[MAX_GROUP_LEN];
    char case_name[MAX_TEST_NAME_LEN];
    char file_name[MAX_FILE_NAME_LEN];
    uint32_t line_num;
    struct unit_test_case_tag *next;
} unit_test_case_t;

typedef struct unit_test_suit_tag {
    unit_func_t *suite_setup;
    unit_func_t *suite_teardown;
    unit_func_t *case_setup;
    unit_func_t *case_teardown;
    char group[MAX_GROUP_LEN];
    struct unit_test_suit_tag *next;
    unit_test_case_t *test_case;
} unit_test_suite_t;

typedef struct {
    bool case_passed;
    uint32_t passed_num;
    uint32_t failed_num;
    uint32_t failed_suite;
    unit_test_suite_t *test_suite;
    unit_test_suite_t *cur_suite;
    unit_test_case_t *cur_case;
} unit_test_ctx_t;

void unit_test_add_suite(unit_test_suite_t *suite);

void unit_test_add_case(unit_test_case_t *tcase);

unit_test_ctx_t *unit_get_ctx();

void unit_test_finish();

int unit_test_run(int argc, char **argv);

#define TEST_SUITE_INIT(groupval) \
    void TEST_##groupval##_SETUP(void); \
    void TEST_##groupval##_TEAR_DOWN(void); \
    void TEST_##groupval##_CASE_SETUP(void); \
    void TEST_##groupval##_CASE_TEAR_DOWN(void); \
    __attribute__((constructor(110)))void TEST_##groupval##_INIT(void) \
    { \
        unit_test_suite_t test_suite =  {\
            .suite_setup = TEST_##groupval##_SETUP, \
            .suite_teardown = TEST_##groupval##_TEAR_DOWN, \
            .case_setup = TEST_##groupval##_CASE_SETUP, \
            .case_teardown = TEST_##groupval##_CASE_TEAR_DOWN, \
            .group = #groupval, \
            .next = NULL, \
            .test_case = NULL \
        };\
        unit_test_add_suite(&test_suite); \
    }

#define TEST_INIT(groupval, nameval, fileval, lineval) \
    void TEST_##groupval##_##nameval##_RUN(); \
    __attribute__((constructor(120)))void TEST_##groupval##_##nameval##_INIT(void) \
    { \
        unit_test_case_t test_case =  {\
            .test_body = TEST_##groupval##_##nameval##_RUN, \
            .group = #groupval, \
            .case_name = #nameval, \
            .file_name = #fileval, \
            .line_num = lineval, \
            .next = NULL, \
        };\
        unit_test_add_case(&test_case); \
    }

#endif