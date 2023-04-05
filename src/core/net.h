/* ********************************
 * Author:       Zhanglele
 * Description:  网络相关接口封装
 * create time: 2022.03.12
 ********************************/
#ifndef NET_H
#define NET_H

#include <stdint.h>

/* 创建socket并进行监听 */
int net_create_listener(int port);

/* 创建socket并连接tcp*/
int net_connect(const char *ip, uint32_t port);

int net_connect_by_host(const char *host, uint32_t port);

/* 设置socket为nonblocking */
int net_set_nonblock(int fd);

/* 获取socket对端的ip和地址 */
int net_get_peer_addr(int fd, char *ip, int len, uint16_t *port);

/* 获取本端socket的ip和地址 */
int net_get_address(int fd, char *ip, int len, uint16_t *port);

/* 读取网络数据 */
int net_read(int fd, char *buf, int count);

/* 发送网络数据 */
int net_write(int fd, char *buf, int buf_len);


#endif