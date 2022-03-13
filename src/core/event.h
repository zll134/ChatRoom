/* ********************************
 * Author:       Zhanglele
 * Description:  epoll的管理模块
 * create time: 2021.11.21
 ********************************/
#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <stdint.h>
#include "rbtree.h"


typedef struct {
    int efd; /* 创建epoll的文件描述符 */
    bool stopped; /* loop是否停止 */
    int timeout; /* epoll超时事件 */
    rbtree_t *events; /* 红黑树进行事件的管理 */
} event_loop_t;

typedef void (*ev_proc_cb)(event_loop_t *loop, int fd, uint32_t mask, void *data);
typedef struct {
    int fd;
    uint32_t mask;
    void *data; /* 局部数据 */
    ev_proc_cb proc; /* 事件回调处理函数 */
} event_t;


/* 创建epoll循环 */
event_loop_t *event_create_loop(void);

void event_run_loop(event_loop_t *loop);
/* 添加epoll事件 */
int event_add(event_loop_t *loop, int fd, uint32_t mask,
    ev_proc_cb handle, void *data);

/* 删除epoll事件 */
int event_del(event_loop_t *loop, int fd);
#endif