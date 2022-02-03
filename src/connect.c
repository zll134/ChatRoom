/* ********************************
 * Author:       Zhanglele
 * Description:  连接管理模块
 * create time: 2021.11.21
 ********************************/
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
#include "event_epoll.h"

#define SERVER_PORT 8089
#define EPOLL_EVENT_SIZE 20

static int init_socket_listener(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        diag_err("generate socket file failed");
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
        diag_err("bind socket failed");
        return -1;
    }
    if (listen(fd, 5) == -1) {
        diag_err("listen fd failed");
        return -1;
    }
    return fd;
}

int connect_init(void)
{
    int lfd = init_socket_listener(SERVER_PORT);
    if (lfd <= 0) {
        diag_err("init socket listener failed");
        return -1;
    }
    diag_info("init socket listener success. fd %d", lfd);

    event_epoll_init();
    event_epoll_add(lfd, EPOLLIN);

    while (1) {
        struct epoll_event events[EPOLL_EVENT_SIZE] = {0};
        int nfds = event_epoll_wait(events, EPOLL_EVENT_SIZE, -1);
        if (nfds < 0) {
            diag_err("event epoll wait failed. nfds %d.", nfds);
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == lfd) {
                int conn_fd = accept(lfd, NULL, 0);
                if (conn_fd < 0) {
                    diag_err("accept fd failed");
                    continue;
                }
                event_epoll_add(conn_fd, EPOLLIN | EPOLLET);
            } else {
                char buf[1024] = {0};
                read(events[i].data.fd, buf, sizeof(buf));
                diag_info("read buf: %s", buf);
            }
        }
    }
    return 0;
}