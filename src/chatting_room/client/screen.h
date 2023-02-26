/* ********************************
 * Author:       Zhanglele
 * Description:  终端屏幕编辑
 * create time: 2022.4.9
 ********************************/

#ifndef SCREEN_H
#define SCREEN_H

/* 从屏幕上读取一行输入字符串, 失败返回-1，成功返回读取的字符数 */
int screen_readline(char *buf, int buf_len);

/* 向屏幕中写入一行，失败返回-1. */
int screen_writeline(const char *line);

/* 向屏幕中输出格式化字符串 */
void screen_write_fmtline(const char *fmt, ...);

#endif