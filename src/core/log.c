/* ********************************
 * Author:       Zhanglele
 * Description:  日志模块管理
 * create time: 2021.10.31
 ********************************/
#include <stdio.h>
#include <stdarg.h>
#include "log.h"

enum {
    CRM_LOG_INFO,
    CRM_LOG_ERR
};

static crm_log_cb log_fn = NULL;

void crm_set_log_callback(crm_log_cb cb)
{
    log_fn = cb;
}

static void crm_log(int severity, const char *buf)
{
    if (log_fn) {
        log_fn(severity, buf);
        return;
    }

    char *severity_str;
    switch (severity) {
        case CRM_LOG_INFO:
            severity_str = "info";
            break;
        case CRM_LOG_ERR:
            severity_str = "error";
            break;
        default:
            break;
    }
    (void)fprintf(stdout, "[%s] %s\n", severity_str, buf);
}

void diag_info(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);
    crm_log(CRM_LOG_INFO, buf);

    va_end(ap);
}

void diag_err(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, ap);
    crm_log(CRM_LOG_ERR, buf);

    va_end(ap);
}