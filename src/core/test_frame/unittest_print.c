/* ********************************
 * Author:       Zhanglele
 * Description:  单元测试打印模块
 * create time: 2023.12.17
 ********************************/
#include "unittest_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void unit_print(uint32_t font_color, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);

    printf("\033[40;%dm%s\033[0m", font_color, buf);

    va_end(ap);
}
