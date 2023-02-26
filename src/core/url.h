/* ********************************
 * Author:       Zhanglele
 * Description:  url模块
 * create time: 2022.12.11
 ********************************/
#ifndef URL_H
#define URL_H

#include "sds.h"

typedef struct {
    sds_t url;      /* url */
    sds_t scheme;   /* 方案，例如http、https */ 
    sds_t user;     /* 某些方案访问需要用户，例如ftp */
    sds_t password; /* 某些方案访问需要密码，例如ftp */
    sds_t host;     /* 宿主服务器的主机名或IP */
    int port;       /* 宿主服务器监听的端口 */
    sds_t path;     /* 宿主服务器上的本地资源名 */
    sds_t params;   /* 某些方案需要参数，允许存在多个参数用:隔开 */
    sds_t query;    /* 传递参数用于激活程序 */
    sds_t frag;     /* 一小片资源名 */
} url_data_t;

int url_parse(const char *urlStr, url_data_t *params)
void url_free(url_data_t *params);
void url_print_info(url_data_t *params);

#endif