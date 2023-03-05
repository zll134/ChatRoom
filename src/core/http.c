/* ********************************
 * Author:       Zhanglele
 * Description:  http模块
 * create time: 2023.01.27
 ********************************/
include "http.h"

#include <stdlib.h>
#include "log.h"
#include "net.h"
#include "pub_def.h"
#include "sds.h"
#include "url.h"
#include "http_request.h"

typedef struct {
    url_data_t conn_params;
    int srv_fd;
} http_client_t;

void http_client_free(http_client_t *client)
{
    if (client->srv_fd != -1) {
        close(client->srv_fd);
    }

    url_free(&client->conn_params);
    free(client);
}

http_client_t *http_client_new(const char *url)
{
    http_client_t *client = calloc(1, sizeof(*client));
    if (client == NULL) {
        return NULL;
    }

    client->srv_fd = -1;
    int ret = url_parse(url, &client->conn_params);
    if (ret != 0) {
        diag_err("Parse url failed, url %s.", url);
        http_client_free(client);
        return NULL;
    }

    return client;
}

int http_connect_server(http_client_t *client)
{
    client->srv_fd = net_connect_by_host(client->conn_params.host, client->conn_params.port);
    if (client->srv_fd == -1) {
        diag_err("Connect to http server failed");
        return TOY_ERR;
    }
    return TOY_OK;
}

int http_request(http_client_t *client)
{
    
}

int http_get(const char *url)
{
    /* 初始化 */
    http_client_t *client = http_client_new(url);
    if (client == NULL) {
        return TOY_ERR;
    }

    /* 连接 */
    int ret = http_connect_server(client);
    if (ret != TOY_OK) {
        return TOY_ERR;
    }

    /* 发送请求 */

    /* 接受响应 */

    /* 释放内存 */
}