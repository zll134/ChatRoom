#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.h"

int net_create_listener(int port)
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

int net_connect(const char *ip, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        diag_err("generate socket file failed");
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);
    addr.sin_port = htons(8089);
    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        diag_err("connect error");
        return -1;
    }
    return fd;
}