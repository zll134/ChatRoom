/* ********************************
 * Author:       Zhanglele
 * Description:  链表数据结构
 * create time: 2022.3.16
 ********************************/
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

list_t *list_create(struct list_ops_s *ops)
{
    list_t *list = (list_t *)malloc(sizeof(*list));
    if (list == NULL) {
        return NULL;
    }
    *list = (list_t){0};
    list->ops = *ops;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    return list;
}

/* 释放链表对象 */
void list_destroy(list_t *list)
{
    if (list == NULL) {
        return;
    }
    list_node_t *head = &list->head;
    while (head->next != &list->tail) {
        list_delete(list, head->next);
    }
    free(list);
}

static list_node_t *create_node(void *value, int len)
{
    list_node_t *node = (list_node_t *)malloc(sizeof(*node) + len);
    if (node == NULL) {
        return NULL;
    }
    node->len = len;
    (void)memcpy(node->data, value, len);
    return node;
}

int list_add_head(list_t *list, void *value, int len)
{
    if ((list == NULL) || (value == NULL)) {
        return -1;
    }

    list_node_t *node = create_node(value, len);
    if (node == NULL) {
        return -1;
    }

    node->next = list->head.next;
    node->prev = &list->head;
    list->head.next->prev = node;
    list->head.next = node;
    list->num++;
    return 0;
}

int list_add_tail(list_t *list, void *value, int len)
{
    if (list == NULL) {
        return -1;
    }
    list_node_t *node = create_node(value, len);
    if (node == NULL) {
        return -1;
    }

    node->prev = list->tail.prev;
    node->next = &list->tail;
    list->tail.prev->next = node;
    list->tail.prev = node;
    list->num++;
    return 0;
}

void *list_find(list_t *list, void *key)
{
    if ((list == NULL) || (list->ops.cmp == NULL)) {
        return NULL;
    }

    list_node_t *node = list->head.next;
    for (int i = 0; i < list->num; i++) {
        if (list->ops.cmp(key, node->data) == 0) {
            return node->data;
        }
    }
    return NULL;
}

int list_delete(list_t *list, list_node_t *node)
{
    if ((node->prev == NULL) || (node->next == NULL)) {
        return -1;
    }

    node->prev->next= node->next;
    node->next->prev = node->prev;
    list->num--;
    free(node);
    return 0;
}

int list_del_head(list_t *list)
{
    if ((list == NULL) || (list->num <= 0)) {
        diag_err("list delete node failed.");
        return -1;
    }
    if (list_delete(list, list->head.next) != 0) {
        diag_err("list delete failed");
        return -1;
    }
    return 0;
}

int list_del_tail(list_t *list)
{
    if ((list == NULL) || (list->num <= 0)) {
        diag_err("list delete node faile.");
        return -1;
    }
    if (list_delete(list, list->tail.prev) != 0) {
        diag_err("list delete failed");
        return -1;
    }
    return 0;
}

void *list_first(list_t *list)
{
    if ((list == NULL) || (list->num == 0)) {
        return NULL;
    }
    return list->head.next->data;
}

#define offsetof(type, member) ((size_t) &((type *)0)->member)
#define container_of(ptr, type, member) ({ \
	    (type *)((char *)ptr - offsetof(type, member)); })

void *list_next(list_t *list, void *data)
{
    if ((list == NULL) || (data == NULL)) {
        diag_err("list_next: input param is invalid.");
        return NULL;
    }

    list_node_t *node = container_of(data, list_node_t, data);
    if (node->next == &list->tail) {
        return NULL;
    }
    return node->next->data;
}