/* ********************************
 * Author:       Zhanglele
 * Description:  客户端入口程序
 * create time: 2022.4.3
 ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "log.h"
#include "net.h"
#include "pub_def.h"
#include "message.h"
#include "screen.h"

static pthread_t g_recv_thread_id;
static int g_client_fd;

void *read_thread_start(void *args)
{
    while (true) {
        //接收, 阻塞等待
        char buf[1024];
        int len = read(g_client_fd, buf, sizeof(buf));
        if (len == -1) {
            ERROR("read error");
            return NULL;
        } else if (len == 0) {
            ERROR("server process closed");
            return NULL;
        }

        msg_hdr_t *hdr = (msg_hdr_t *)buf;
        printf("client recv msg: %u:%u\n", hdr->type, hdr->len);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int g_client_fd = net_connect("127.0.0.1", SERVER_PORT);
    if (g_client_fd == -1) {
        ERROR("connet tcp failed");
        return -1;
    }
    /* pthread_create(&g_recv_thread_id, NULL, read_thread_start, NULL);
    pthread_detach(g_recv_thread_id); */

    screen_writeline("Please enter your name: ");
    char name[1024] = {0};
    if (screen_readline(name, sizeof(name)) <= 0) {
        ERROR("read screen line failed");
        return -1;
    }

    msg_login_req_t login_req = {0};
    strcpy(login_req.name, name);
    if (msg_send(g_client_fd, MSG_LOGIN, &login_req, sizeof(login_req)) != 0) {
        return -1;
    }

    char ip[32] = {0};
    uint16_t port;
    if (net_get_address(g_client_fd, ip, sizeof(ip), &port) != 0) {
        ERROR("get address failed");
        return -1;
    }

    /* 显示用户的ip和地址 */
    screen_write_fmtline("Your are: %s:%u.\n", ip, port);
    msg_chat_req_t chat_msg = {0};
    strcpy(chat_msg.name, name);
    while (true) {
        screen_write_fmtline("> ");
        char buf[1024] = {0};
        if (screen_readline(buf, sizeof(buf)) <= 0) {
            ERROR("read screen line failed");
            return -1;
        }

        strcpy(chat_msg.buf, buf);
        if (msg_send(g_client_fd, MSG_CHAT, &chat_msg, sizeof(chat_msg)) != 0) {
            printf("data send failed");
            continue;
        }
    }
    return 0;
}