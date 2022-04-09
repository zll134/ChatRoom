/* ********************************
 * Author:       Zhanglele
 * Description:  服务端程序
 * create time: 2022.4.5
 ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "pub.h"
#include "log.h"
#include "thread_pool.h"
#include "event.h"
#include "net.h"
#include "event.h"
#include "list.h"
#include "message.h"

#define EPOLL_EVENT_SIZE 20

typedef struct {
    uint32_t ip;
    uint16_t port;
    int fd;
    char name[USER_NAME_LEN]; /* 用户的名称 */
} client_t;

struct {
    event_loop_t *loop; /* epoll循环 */
    list_t *clients; /* 客户端的信息 */
} g_server;

static int client_compare(void *data1, void *data2)
{
    client_t *client1 = (client_t *)data1;
    client_t *client2 = (client_t *)data2;
    if (strcmp(client1->name, client2->name) == 0) {
        return 0;
    }
    return -1;
}

static void login_msg_proc(int fd, void *buf, int len)
{
    msg_login_req_t *login_req = (msg_login_req_t *)buf;
    client_t client = {0};
    diag_info("recv login msg, name %s", login_req->name);
    strcpy(client.name, login_req->name);
    msg_login_resp_t login_resp = {0};
    if (list_find(g_server.clients, &client) != NULL) {
        diag_info("user is exist.");
        login_resp.result = LOGIN_USER_EXIST;
        msg_send(fd, MSG_LOGIN, &login_resp, sizeof(login_resp));
        return;
    }
    client.fd = fd;
    list_add_head(g_server.clients, &client, sizeof(client));
}

static void chat_msg_proc(int fd, void *buf, int len)
{
    msg_chat_req_t *chat_req = (msg_chat_req_t *)buf;
    diag_info("recv chat msg, name %s, content %s", chat_req->name, chat_req->buf);
    void *ptr = list_first(g_server.clients);
    while (ptr != NULL) {
        client_t *client = (client_t *)ptr;
        if (strcmp(client->name, chat_req->name) != 0) {
            msg_send(fd, MSG_CHAT, buf, len);
        }
        ptr = list_next(g_server.clients, ptr);
    }
}

static void receive_cb(event_loop_t *loop, int fd, uint32_t mask, void *data)
{
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf));
    if (len == -1) {
        diag_err("read buff error");
        return;
    } else if (len == 0) {
        diag_info("client close fd");
        event_del(loop, fd);

        /* 需要从list中删除 */
        return;
    }

    msg_hdr_t *hdr = (msg_hdr_t *)buf;
    diag_info("recv msg, msg type %d msg len %d", hdr->type, hdr->len);
    switch (hdr->type) {
        case MSG_LOGIN:
            login_msg_proc(fd, hdr + 1, hdr->len);
            break;
        case MSG_CHAT:
            chat_msg_proc(fd, hdr + 1, hdr->len);
            break;
        default:
            break;
    }
}

static void accept_cb(event_loop_t *loop, int fd, uint32_t mask, void *data)
{
    int conn_fd = accept(fd, NULL, 0);
    diag_info("new client connet fd %d connfd %d", fd, conn_fd);
    if (conn_fd < 0) {
        diag_err("accept fd failed");
        return;
    }

    event_add(g_server.loop, conn_fd, EPOLLIN,
              receive_cb, NULL);
}

int connect_init(void)
{
    int lfd = net_create_listener(SERVER_PORT);
    if (lfd <= 0) {
        diag_err("init socket listener failed");
        return -1;
    }

    net_set_nonblock(lfd);
    diag_info("init socket listener success. fd %d", lfd);
    g_server.loop = event_create_loop();
    event_add(g_server.loop, lfd, EPOLLIN, accept_cb, NULL);
    event_run_loop(g_server.loop);
    return 0;
}

int main(int argc, char **argv)
{
    struct list_ops_s ops = {
        .cmp = client_compare
    };
    g_server.clients = list_create(&ops);
    if (g_server.clients == NULL) {
        diag_err("create clients failed");
        return -1;
    }

    connect_init();
    return 0;
}