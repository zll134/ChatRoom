/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试打印模块
 * create time: 2023.12.17
 ********************************/
#ifndef UNITTEST_PRINT_H
#define UNITTEST_PRINT_H

#include <stdint.h>
#include <stdbool.h>

enum {
    FONT_RED = 31,
    FONT_GREEN = 32,
    FONT_WHITE = 37,
};

void unit_print(uint32_t font_color, const char *fmt, ...);

#endif