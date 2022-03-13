/* ********************************
 * Author:       Zhanglele
 * Description:  epoll的管理模块
 * create time: 2021.11.21
 ********************************/
#include "event.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "log.h"
#include "rbtree.h"

#define EPOLL_SIZE 256
#define EPOLL_EVENT_SIZE 20

static int event_cmp(void *data1, void *data2)
{
    return ((event_t *)data1)->fd > ((event_t *)data2)->fd;
}

event_loop_t *event_create_loop()
{
    event_loop_t *loop = (event_loop_t *)malloc(sizeof(*loop));
    loop->stopped = false;
    loop->timeout = -1;
    loop->efd = epoll_create(EPOLL_SIZE);
    if (loop->efd <= 0) {
        free(loop);
        diag_err("create epoll failed");
        return NULL;
    }

    struct rbtree_ops_s ops = {
        event_cmp,
        NULL
    };
    loop->events = rbtree_create(&ops);
    if (loop->events == NULL) {
        return NULL;
    }

    return loop;
}

void event_delete_loop(event_loop_t *loop)
{
    rbtree_destroy(loop->events);
    free(loop);
}

void event_stop_loop(event_loop_t *loop)
{
    loop->stopped = true;
}

void event_run_loop(event_loop_t *loop)
{
    while (!loop->stopped) {
        struct epoll_event ees[EPOLL_EVENT_SIZE];
        int n = epoll_wait(loop->efd, ees, EPOLL_EVENT_SIZE, loop->timeout);
        if (n < 0) {
            diag_err("epoll wait failed");
            return;
        }

        for (int i = 0; i < n; i++) {
            event_t key = {0};
            key.fd = ees[i].data.fd;
            rbtree_node_t *node = rbtree_find(loop->events, &key);
            if (node == NULL) {
                continue;
            }

            event_t *event = (event_t *)node->data;
            event->proc(loop, event->fd, event->mask, event->data);
        }
    }
}

int event_add(event_loop_t *loop, int fd, uint32_t mask,
    ev_proc_cb handle, void *data)
{
    event_t event = {
        .fd = fd,
        .mask = mask,
        .proc = handle,
        .data = data
    };
    rbtree_insert(loop->events, &event, sizeof(event));

    struct epoll_event ev = {0};
    ev.events = mask;
    ev.data.fd = fd;
    if (epoll_ctl(loop->efd, EPOLL_CTL_ADD, fd, &ev) != 0) {
        diag_err("epoll add failed");
        return -1;
    }
    return 0;
}

int event_del(event_loop_t *loop, int fd)
{
    event_t key = {
        .fd = fd
    };
    rbtree_delete(loop->events, &key);

    if (epoll_ctl(loop->efd, EPOLL_CTL_DEL, fd, NULL) != 0) {
        diag_err("epoll del failed");
        return -1;
    }
    close(fd);
    return 0;
}