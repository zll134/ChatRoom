/* ********************************
 * Author:       Zhanglele
 * Description:  epoll的管理模块
 * create time: 2021.11.21
 ********************************/
#ifndef EVENT_EPOLL_H
#define EVENT_EPOLL_H

int event_epoll_init(void);

int event_epoll_add(int efd, int fd, uint32_t events);

int event_epoll_wait(int efd, struct epoll_event *events,
    int size, int timeout);

#endif