/* ********************************
 * Author:       Zhanglele
 * Description:  http模块
 * create time: 2023.01.27
 ********************************/
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "net.h"
#include "pub_def.h"
#include "sds.h"
#include "url.h"
#include "http_request.h"

typedef struct {
    url_data_t conn_params;
    http_request_t req;
    int srv_fd;
} http_client_t;

void http_client_free(http_client_t *client)
{
    if (client->srv_fd != -1) {
        close(client->srv_fd);
        client->srv_fd = -1;
    }

    url_free(&client->conn_params);
    http_request_release(&client->req);
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
        ERROR("Parse url failed, url %s.", url);
        http_client_free(client);
        return NULL;
    }
    ret = http_request_init(&client->req);
    if (ret != TOY_OK) {
        http_client_free(client);
        return NULL;
    }
    return client;
}

int http_connect_server(http_client_t *client)
{
    client->srv_fd = net_connect_by_host(client->conn_params.host, client->conn_params.port);
    if (client->srv_fd == -1) {
        ERROR("Connect to http server failed");
        return TOY_ERR;
    }
    return TOY_OK;
}

int http_send_req(http_client_t *client)
{
    int ret = net_write(client->srv_fd, client->req.msg, sds_get_len(client->req.msg));
    if (ret != sds_get_len(client->req.msg)) {
        return TOY_ERR;
    }
    return TOY_OK;
}

int http_recv_req(http_client_t *client)
{
    char response[4096] = {0};
    net_read(client->srv_fd, response, sizeof(response));
    printf("%s\n", response);
    return TOY_OK;
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
        http_client_free(client);
        return TOY_ERR;
    }

    /* 构造请求报文 */
    ret = http_request_build_msg(HTTP_GET, &client->req, &client->conn_params);
    if (ret != TOY_OK) {
        http_client_free(client);
        return TOY_ERR;
    }

    /* 发送请求报文 */
    ret = http_send_req(client);
    if (ret != TOY_OK) {
        http_client_free(client);
        return TOY_ERR;
    }
    /* 接受响应 */
    ret = http_recv_req(client);
    if (ret != TOY_OK) {
        http_client_free(client);
        return TOY_ERR;
    }
    /* 释放内存 */
    http_client_free(client);
    return TOY_OK;
}