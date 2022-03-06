
#ifndef NET_H
#define NET_H

/* 创建socket并进行监听 */
int net_create_listener(int port);

/* 创建socket并连接tcp*/
int net_connect(const char *ip, int port);
#endif