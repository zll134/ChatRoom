/* ********************************
 * Author:       Zhanglele
 * Description:  日志模块管理
 * create time: 2021.10.31
 ********************************/

#ifndef LOG_H
#define LOG_H

typedef void (*crm_log_cb)(int severity, const char *log);

/* 通过该函数定制化日志存储接口; 默认输出到标准错误中 */
void crm_set_log_callback(crm_log_cb cb);

/* 提示日志接口 */
void diag_info(const char *fmt, ...);

/* 错误日志接口 */
void diag_err(const char *fmt, ...);

#define INFO(args...) \
    do { \
        diag_info(args);\
    } while(0)

#define ERROR(args...) \
    do { \
        diag_err(args);\
    } while(0)

/* 调试日志 */
#ifdef DEBUG_MODE
#define TRACE(s, args...) \
    do { \
        diag_info("[trace] "s, ## args);\
    } while(0)
#else
#define TRACE(args...)
#endif

#endif