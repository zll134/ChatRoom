/* ********************************
 * Author:       Zhanglele
 * Description:  http 请求管理
 * create time: 2023.02.05
 ********************************/
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "sds.h"
#include "t_list.h"
#include "http_pub.h"

typedef struct {
    sds_t start_line; /* 起始行 */
    list_t headers; /* 首部 */
    sds_t body; /* 实体部分 */
} http_request_t;

int http_request_init(http_request_t *req);

int http_request_free(http_request_t *req);

int http_request_set_start_line(http_request_t *req, HTTP_METHOD_TYPE_E method,
    url_data_t *url_data);

#endif