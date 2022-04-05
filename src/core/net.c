/* ********************************
 * Author:       Zhanglele
 * Description:  网络相关接口封装
 * create time: 2022.03.12
 ********************************/
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
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

int net_set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        diag_err("get file flags failed");
        return -1;
    }
    flags = flags | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        diag_err("fcntl set nonblock failed");
        return -1;
    }
    return 0;
}

int net_get_peer_addr(int fd, char *ip, int len, uint16_t *port)
{
    if ((ip == NULL) || (port == NULL)) {
        diag_err("input para is invalid");
        return -1;
    }

    struct sockaddr_in peer_addr = {0};
    socklen_t peer_addr_len = sizeof(peer_addr);
    if (getpeername(fd, (struct sockaddr*)&peer_addr, &peer_addr_len) == -1) {
        diag_err("get peer name failed");
        return -1;
    }

    if (peer_addr.sin_family != AF_INET) {
        diag_err("server only support ipv4.");
        return -1;
    }
    *port = peer_addr.sin_port;
    inet_ntop(AF_INET,(void*)&(peer_addr.sin_addr), ip, len);
    return 0;
}

int net_get_address(int fd, char *ip, int len, uint16_t *port)
{
    if ((ip == NULL) || (port == NULL)) {
        diag_err("input para is invalid");
        return -1;
    }

    struct sockaddr_in addr = {0};
    socklen_t addr_len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr*)&addr, &addr_len) == -1) {
        diag_err("get peer name failed");
        return -1;
    }

    if (addr.sin_family != AF_INET) {
        diag_err("server only support ipv4.");
        return -1;
    }
    *port = addr.sin_port;
    inet_ntop(AF_INET,(void*)&(addr.sin_addr), ip, len);
    return 0;
}