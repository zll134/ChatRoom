/* ********************************
 * Author:       Zhanglele
 * Description:  http 请求管理
 * create time: 2023.02.05
 ********************************/

#include "http_request.h"

#include "sds.h"
#include "t_list.h"
#include "http_pub.h"

int http_request_init(http_request_t *req)
{
    list_init(req->headers);
    req->start_line = NULL;
    req->body = NULL;
}

int http_request_free(http_request_t *req)
{
    if (req->body != NULL) {
        sds_free(req->body);
    }
    if (req->start_line != NULL) {
        sds_free(req->start_line);
    }
}

int http_request_set_start_line(http_request_t *req, HTTP_METHOD_TYPE_E method,
    url_data_t *url_data);
{
    if (method == HTTP_GET) {
        
    }
}