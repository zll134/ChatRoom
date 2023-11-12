/* ********************************
 * Author:       Zhanglele
 * Description:  终端屏幕编辑
 * create time: 2022.4.9
 ********************************/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "log.h"

enum KEY_ACTION{
	KEY_NULL = 0,	    /* NULL */
	CTRL_A = 1,         /* Ctrl+a */
	CTRL_B = 2,         /* Ctrl-b */
	CTRL_C = 3,         /* Ctrl-c */
	CTRL_D = 4,         /* Ctrl-d */
	CTRL_E = 5,         /* Ctrl-e */
	CTRL_F = 6,         /* Ctrl-f */
	CTRL_H = 8,         /* Ctrl-h */
	TAB = 9,            /* Tab */
    NEW_LINE = 10,           /* Enter newline */
	CTRL_K = 11,        /* Ctrl+k */
	CTRL_L = 12,        /* Ctrl+l */
	ENTER = 13,         /* Enter */
	CTRL_N = 14,        /* Ctrl-n */
	CTRL_P = 16,        /* Ctrl-p */
	CTRL_T = 20,        /* Ctrl-t */
	CTRL_U = 21,        /* Ctrl+u */
	CTRL_W = 23,        /* Ctrl+w */
	ESC = 27,           /* Escape */
    SPACE = 32,         /* Space */
	BACKSPACE =  127    /* Backspace */
};
static bool is_readable_char(char c)
{
    if (c >= SPACE && c < BACKSPACE) {
        return true;
    }
    return false;
}
int screen_readline(char *buf, int buf_len)
{
    int count = 0;
    while (count < buf_len - 1) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) <= 0) {
            ERROR("read char from screen failed");
            return -1;
        }

        /* 可读字符直接传出 */
        if (is_readable_char(ch)) {
            buf[count++] = ch;
        }

        if (ch == BACKSPACE) {
            count = (count > 0) ? (count - 1) : 0;
        }

        /* 换行 */
        if ((ch == ENTER) || (ch == NEW_LINE)) {
            break;
        }
    }

    buf[count] = '\0';
    return count;
}


int screen_writeline(const char *line)
{
    if (line == NULL) {
        return -1;
    }

    const char *pos = line;
    int len = strlen(line);
    while (len > 0) {
        int nwrite = write(STDOUT_FILENO, pos, len);
        if (nwrite <= 0) {
            ERROR("screen writeline failed");
            return -1;
        }
        len -= nwrite;
        pos = pos + nwrite;
    }
    return 0;
}

void screen_write_fmtline(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);
    screen_writeline(buf);

    va_end(ap);
}