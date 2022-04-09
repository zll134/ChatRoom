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
#include "pub.h"
#include "message.h"

static pthread_t g_recv_thread_id;
static int g_client_fd;

static void *read_thread_start(void *args)
{
    while (true) {
        //接收, 阻塞等待
        char buf[1024];
        int len = read(g_client_fd, buf, sizeof(buf));
        if (len == -1) {
            diag_err("read error");
            return NULL;
        } else if (len == 0) {
            diag_err("server process closed");
            return NULL;
        }

        msg_hdr_t *hdr = (msg_hdr_t *)buf;
        printf("%u:%u\n", hdr->type, hdr->len);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int g_client_fd = net_connect("127.0.0.1", SERVER_PORT);
    if (g_client_fd == -1) {
        diag_err("connet tcp failed");
        return -1;
    }
    pthread_create(&g_recv_thread_id, NULL, read_thread_start, NULL);
    pthread_detach(g_recv_thread_id);

    printf("Please enter your name: ");
    char name[1024] = {0};
    fscanf(stdin, "%s", name);
    printf("\nname: %s\n", name);
    msg_login_req_t login_req = {0};
    strcpy(login_req.name, name);
    if (msg_send(g_client_fd, MSG_LOGIN, &login_req, sizeof(login_req)) != 0) {
        return -1;
    }

    char ip[32] = {0};
    uint16_t port;
    if (net_get_address(g_client_fd, ip, sizeof(ip), &port) != 0) {
        diag_err("get address failed");
        return -1;
    }

    /* 显示用户的ip和地址 */
    printf("Your are: %s:%u.\n", ip, port);

    while (true) {
        printf("> ");
        char buf[1024] = {0};
        scanf("%s", buf);
        if (msg_send(g_client_fd, MSG_CHAT, buf, sizeof(buf)) != 0) {
            printf("data send failed");
            continue;
        }
    }
    return 0;
}