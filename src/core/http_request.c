/* ********************************
 * Author:       Zhanglele
 * Description:  http 请求管理
 * create time: 2023.02.05
 ********************************/

#include "http_request.h"

#include <stdlib.h>
#include <stdbool.h>

#include "pub_def.h"
#include "log.h"
#include "sds.h"
#include "t_list.h"
#include "url.h"
#include "http_pub.h"

#define HTTP_REQUEST_MSG_INIT_LEN 4096

typedef struct {
    HTTP_METHOD_TYPE_E method;
    const char *method_str;
} http_method_map_t;

static http_method_map_t g_method_map[] = {
    {HTTP_GET, "GET"},
    /* 其他方法待添加 */
};

#define HTTP_CRLF "\r\n"

const char *http_get_method_str(HTTP_METHOD_TYPE_E method)
{
    for (int i = 0; i < ARRAY_SIZE(g_method_map); i++) {
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

int http_request_init(http_request_t *req)
{
    req->msg = sds_new_with_len(NULL, HTTP_REQUEST_MSG_INIT_LEN);
    diag_info("zhanglele test: http_request_init obj %s, %p objlen %u.", req->msg, req->msg, sds_get_len(req->msg));
    if (req->msg == NULL) {
        diag_err("[http request] New sds failed.");
        return TOY_ERR;
    }
    return TOY_OK;
}

int http_request_release(http_request_t *req)
{
    sds_free(req->msg);
    req->msg = NULL;
}

/**
 * biref:
 *   设置报文的start line.
 * 
 * details:
 *   请求行的格式：
 *       <method> <requests_url> <version>
 *   method: 表示请求方法，例如GET, PUT等
 *   requrests_url: 请求的资源资源的路径
 *   version: 报文所使用的http的版本
 * 
 *   例如： GET /test/test.txt  HTTP/1.1
 */
int http_request_set_start_line(http_request_t *req, HTTP_METHOD_TYPE_E method,
    url_data_t *url_data)
{
    diag_info("zhanglele test: http_request_set_start_line start, path: %s.", url_data->path);
    const char *method_str = http_get_method_str(method);
    req->msg = sds_vcat(req->msg, method_str, " ", (char *)url_data->path,
                        " HTTP/1.1\r\n");
    if (req->msg == NULL) {
        return TOY_ERR;
    }
    return TOY_OK;
}

/**
 *  brief:
 *     添加header到请求报文
 *  details:
 *    header的格式：
 *       key: value\r\n
 *   例如： Host: www.jiedev.com\r\n
 */
int http_request_add_header(http_request_t *req, const char *key, const char*value)
{
    req->msg = sds_vcat(req->msg, key, ": ", value, HTTP_CRLF);
    if (req->msg == NULL) {
        return TOY_ERR;
    }
    return TOY_OK;
}

/* 设置请求报文的headers部分字段 */
int http_request_set_headers(http_request_t *req, url_data_t *url_data)
{
    int ret = http_request_add_header(req, "Host", url_data->host);
    if (ret != TOY_OK) {
        return TOY_ERR;
    }

    req->msg = sds_cat(req->msg, HTTP_CRLF);
    if (req->msg == NULL) {
        return TOY_ERR;
    }
    return TOY_OK;
}

/* 设置报文的body字段 */
int http_request_set_body(http_request_t *req)
{
    return TOY_OK;
}

int http_request_build_msg(uint32_t method, http_request_t *req,
    url_data_t *url_data)
{
    diag_err("zhanglele test: http_request_build_msg method %u msg %s %p msglen %u.",
        method, req->msg, req->msg, sds_get_len(req->msg));
    if (!http_is_method_valid(method)) {
        diag_err("[http request] Http method is invalid.");
        return TOY_ERR;
    }

    // 设置start_line部分
    int ret = http_request_set_start_line(req, method, url_data);
    if (ret != TOY_OK) {
        sds_free(req->msg);
        return TOY_ERR;
    }
    diag_info("zhanglele test: http_request_set_start_line end, msg:%s.", req->msg);
    diag_info("zhanglele test: http_request_set_headers start");

    // 设置headers部分
    ret = http_request_set_headers(req, url_data);
    if (ret != TOY_OK) {
        sds_free(req->msg);
        return TOY_ERR;
    }
    diag_info("zhanglele test: http_request_set_headers end");
    // 设置body部分
    ret = http_request_set_body(req);
    if (ret != TOY_OK) {
        sds_free(req->msg);
        return TOY_ERR;
    }

    sds_free(req->msg);
    return TOY_OK;
}