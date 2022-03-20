/* ********************************
 * Author:       Zhanglele
 * Description:  链表数据结构
 * create time: 2022.3.16
 ********************************/
#ifndef LIST_H
#define LIST_H

typedef struct list_ops_s {
    int (*cmp)(void *data1, void *data2); /* 插入数据比较，返回负值; */
} list_ops_t;

typedef struct list_node_s {
    struct list_node_s *prev;
    struct list_node_s *next;
    void *data;
    int len;
} list_node_t;

typedef struct {
    struct list_ops_s ops;
    list_node_t head;
    list_node_t tail;
    int num;
} list_t;

/* 创建链表对象 */
list_t *list_create(struct list_ops_s *ops);

/* 将节点插入链表头 */
int list_add_head(list_t *list, void *value, int len);

/* 将链表头插入链表尾部 */
int list_add_tail(list_t *list, void *value, int len);

/* 根据key查找链表节点 */
list_node_t *list_find(list_t *list, void *key);

/* 删除具体的链表节点 */
int list_delete(list_t *list, list_node_t *node);

/* 删除链表首部的节点 */
int list_del_head(list_t *list);

/* 删除链表尾部的节点 */
int list_del_tail(list_t *list);

/* 释放链表对象 */
void list_destroy(list_t *list);
#endif