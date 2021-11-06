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
void crm_diag_info(const char *fmt, ...);

/* 错误日志接口 */
void crm_diag_err(const char *fmt, ...);

#endif