/* ********************************
 * Author:       Zhanglele
 * Description:  http 请求管理
 * create time: 2023.02.05
 ********************************/

#include "http_request.h"

#include "pub_def.h"
#include "sds.h"
#include "t_list.h"
#include "http_pub.h"

#define HTTP_REQUEST_MSG_INIT_LEN 4096
typedef struct {
    sds_t msg; /* http请求消息 */
} http_request_t;

typedef struct {
    HTTP_METHOD_TYPE_E method;
    const char *method_str;
} http_method_map_t;

static http_method_map_t g_method_map[] = {
    {HTTP_GET, "GET"},
    /* 其他方法待添加 */
};

const char *http_get_method_str(HTTP_METHOD_TYPE_E method)
{
    for (int i = 0; i < ARRAY_SIZE(g_method_map)) {
        if (g_method_map[i].method == method) {
            return g_method_map[i].method_str;
        }
    }
    return "";
}

bool http_is_method_valid(HTTP_METHOD_TYPE_E method)
{
    if (method >= HTTP_INVALID_METHOD) {
        return false;
    }
    return true;
}


/**
 * 请求行的格式：
 *     <method> <requests_url> <version>
 * method: 表示请求方法，例如GET, PUT等
 * requrests_url: 请求的资源资源的路径
 * version: 报文所使用的http的版本
 * 
 * 例如：
 *     GET /test/test.txt  HTTP/1.1
 */
int http_request_set_start_line(http_request_t *req, HTTP_METHOD_TYPE_E method,
    url_data_t *url_data);
{
    const char *method_str = http_get_method_str(method);

    req->msg = sds_cat(req->msg, method_str);
    if (req->msg == NULL) {
        return TOY_ERR
    }

    req->msg = sds_cat(req->msg, " ");
    if (req->msg == NULL) {
        return TOY_ERR
    }

    req->msg = sds_cat(req->msg, url_data->path);
    if (req->msg == NULL) {
        return TOY_ERR
    }

    req->msg = sds_cat(req->msg, "HTTP/1.1\r\n");
    if (req->msg == NULL) {
        return TOY_ERR
    }
    return TOY_OK;
}

int http_request(HTTP_METHOD_TYPE_E method, url_data_t *url_data)
{
    if (http_is_method_valid(method)) {
        diag_err("[http request] Http method is invalid.")
        return TOY_ERR;
    }

    http_request_t req = {0};
    req.msg = sds_new_with_len(HTTP_REQUEST_MSG_INIT_LEN);
    if (req.msg == NULL) {
        diag_err("[http request] New sds failed.")
        return TOY_ERR;
    }

    int ret =http_request_set_start_line(req, method, url_data);
    if (ret != TOY_OK) {
        sds_free(req.msg);
        return TOY_ERR;
    }l


    sds_free(req.msg);
}