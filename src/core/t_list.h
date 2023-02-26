/* ********************************
 * Author:       Zhanglele
 * Description:  链表数据结构
 * create time: 2022.3.16
 ********************************/
#ifndef T_LIST_H
#define T_LIST_H

typedef struct list_ops_s {
    int (*cmp)(void *data1, void *data2); /* 插入数据比较，返回负值; */
} list_ops_t;

typedef struct list_node_s {
    struct list_node_s *prev;
    struct list_node_s *next;
    int len;
    char data[0];
} list_node_t;

typedef struct {
    struct list_ops_s ops;
    list_node_t head;
    list_node_t tail;
    int num;
} list_t;

/* 初始化链表对象 */
void list_init(list_t *list, struct list_ops_s *ops);

/* 释放链表对象 */
void list_destroy(list_t *list);

/* 在链表首部插入链表节点 */
int list_add_head(list_t *list, void *value, int len);

/* 在链表尾部插入链表节点，key和value定义在相同的结构体中 */
int list_add_tail(list_t *list, void *value, int len);

/* 根据key查找链表节点中存储数据, key和value使用相同的结构体 */
void *list_find(list_t *list, void *key);

/* 删除链表节点 */
int list_delete(list_t *list, list_node_t *node);

/* 删除链表首部的链表节点 */
int list_del_head(list_t *list);

/* 删除链表尾部的链表节点 */
int list_del_tail(list_t *list);

/* 返回链表第一个节点中存储中的数据 */
void *list_first(list_t *list);

/*
 * 返回下一个节点中的数据。
 * @data: 链表节点的数据，使用者需要确保入参的正确性
 * @return: 下一个链表节点中的数据
 */
void *list_next(list_t *list, void *data);

#endif