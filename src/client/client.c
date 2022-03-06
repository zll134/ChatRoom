#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "log.h"
#include "net.h"

#define SERVER_PORT 8089

int main(int argc, char **argv)
{
    int fd = net_connect("127.0.0.1", SERVER_PORT);
    if (fd == -1) {
        diag_err("connet tcp failed");
        return -1;
    }
    diag_info("client connect success, fd %d", fd);
    while (true) {
        char buf[1024] = {0};
        fgets(buf, sizeof(buf), stdin);
        write(fd, buf, strlen(buf));
        diag_info("client write buf %s", buf);
        //接收, 阻塞等待
        int len = read(fd, buf, sizeof(buf));
        if (len == -1) {
             diag_err("read error");
             return -1;
        }
        diag_info("client recv %s\n", buf);
    }
    return 0;
}