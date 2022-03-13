
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "log.h"
#include "thread_pool.h"
#include "event.h"
#include "net.h"
#include "event.h"

#define SERVER_PORT 8089
#define EPOLL_EVENT_SIZE 20

struct {
    event_loop_t *loop; /* epoll循环 */
} g_server;

static void accept_event_proc(event_loop_t *loop, int fd, uint32_t mask, void *data)
{
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf));
    if (len == -1) {
        diag_err("read buff error");
        return;
    } else if (len == 0) {
        diag_info("client close fd");
        close(fd);
        event_del(loop, fd);
        return;
    }
    diag_info("server recev data buf", buf);
    write(fd, buf, sizeof(buf));
}

static void socket_event_proc(event_loop_t *loop, int fd, uint32_t mask, void *data)
{
    int conn_fd = accept(fd, NULL, 0);
    diag_info("new client connet fd %d connfd %d", fd, conn_fd);
    if (conn_fd < 0) {
        diag_err("accept fd failed");
        return;
    }
    event_add(g_server.loop, conn_fd, EPOLLIN,
        accept_event_proc, NULL);
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
    event_add(g_server.loop, lfd, EPOLLIN, socket_event_proc, NULL);
    event_run_loop(g_server.loop);
    return 0;
}

int main(int argc, char **argv)
{
    connect_init();
}