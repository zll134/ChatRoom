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

#define THREAD_NUM 5
#define JOB_NUM 10

#define SERVER_PORT 8089

#define EPOLL_SIZE 15
#define EPOLL_EVENT_SIZE 15

int g_job_index[JOB_NUM] = {0};

void *thread_run(void *val)
{
    int index = *(int *)val;
    printf("thread start run, index is %d\n", index);
    sleep(1);
    return NULL;
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        diag_err("generate socket file failed");
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
        printf("bind socket failed\n");
        return -1;
    }
    listen(fd, 5);
    diag_info("listen fd success");
    int efd = epoll_create(EPOLL_SIZE);
    struct epoll_event ev = {0};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        diag_err("epoll_ctl: listen_sock");
        return 0;
    }
    while (1) {
        struct epoll_event events[EPOLL_EVENT_SIZE] = {0};
        int nfds = epoll_wait(efd, events, EPOLL_EVENT_SIZE, 1000);
        if (nfds < 0) {
            diag_err("nfds < 0\n");
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == fd) {
                diag_info("start accept");
                socklen_t addrlen = sizeof(addr);
                int conn_fd = accept(fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
                if (conn_fd < 0) {
                    diag_err("accept fd failed");
                    continue;
                }
                diag_info("accept fd success");
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_fd;
                epoll_ctl(efd, EPOLL_CTL_ADD, conn_fd, &ev);
            } else {
                char buf[1024] = {0};
                read(events[i].data.fd, buf, sizeof(buf));
                diag_info("read buf: %s", buf);
            }
        }
    }
    return 0;
}