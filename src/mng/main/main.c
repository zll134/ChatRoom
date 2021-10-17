#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "thread_pool.h"

#define THREAD_NUM 5
#define JOB_NUM 10

#define SERVER_IP "192.168.75.143"
#define SERVER_PORT 8088

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
        printf("generate socket file failed\n");
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr.sin_port = htons(SERVER_PORT);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
        printf("bind socket failed\n");
        return -1;
    }
    listen(fd, 5);
    printf("listen fd success\n");
    while (1) {
        printf("start accept\n");
        socklen_t addrlen = sizeof(addr);
        int new_fd = accept(fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
        if (new_fd < 0) {
            printf("accept fd failed\n");
            continue;
        }
        printf("accept fd success\n");
        char buf[1024] = {0};
        read(new_fd, buf, sizeof(buf));
        printf("read buf:%s\n", buf);
    }
    return 0;
}