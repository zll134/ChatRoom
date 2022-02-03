/* ********************************
 * Author:       Zhanglele
 * Description:  红黑树基本数据结构
 * create time: 2021.09.21
 ********************************/

#ifndef RBTREE_H
#define RBTREE_H

struct rbtree_ops_s {
    int (*cmp)(void *data1, void *data2); /* 插入数据比较，返回负值; */
    void *(*dup)(void *data); /* 插入数据拷贝 */
    const char*(*dump)(void *data);
};

typedef struct rbtree_node_s rbtree_node_t;

typedef struct {
    rbtree_node_t *root;
    rbtree_node_t *sentinel;
    struct rbtree_ops_s ops;
    int num;
} rbtree_t;

/* 红黑树生成函数 */
rbtree_t *rbtree_create(struct rbtree_ops_s *ops);

/* 红黑树中节点数据 */
int rbtree_insert(rbtree_t *tree, void *data);

/* 红黑树中根据key查找value, key和value共一个结构体 */
rbtree_node_t *rbtree_find(rbtree_t *tree, void *key);

/* 删除红黑树节点数据. 返回0，表示插入成功 */
int rbtree_delete(rbtree_t *tree, void *data);

void rbtree_dump(rbtree_t *tree, rbtree_node_t *node, int depth);

/* 红黑树销毁函数 */
void rbtree_destroy(rbtree_t *tree);

#endif