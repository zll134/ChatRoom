/* ********************************
 * Author:       Zhanglele
 * Description:  红黑树基本数据结构
 * create time: 2021.09.21
 ********************************/
#include "rbtree.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "log.h"

#define RED 0
#define BLACK 1

struct rbtree_node_s {
    rbtree_node_t *parent;
    rbtree_node_t *left;
    rbtree_node_t *right;
    uint8_t color;
    void *data;
};

static inline void rbt_set_color(rbtree_node_t *node, uint8_t color)
{
    node->color = color;
}

static inline bool rbt_is_red(rbtree_node_t *node)
{
    return (node->color == RED);
}

rbtree_t *rbtree_create(struct rbtree_ops_s *ops)
{
    rbtree_t *tree = (rbtree_t *)malloc(sizeof(*tree));
    if (tree == NULL) {
        diag_err("create rbtree failed");
        return NULL;
    }
    tree->sentinel = (rbtree_node_t *)malloc(sizeof(*tree->sentinel));
    if (tree->sentinel == NULL) {
        free(tree);
        diag_err("create rbtree sentinel failed");
        return NULL;
    }
    rbt_set_color(tree->sentinel, BLACK);
    tree->ops = *ops;
    tree->root = tree->sentinel;
    return tree;
}

static void rbtree_insert_node(rbtree_t *tree, rbtree_node_t *node)
{
    /* 空树则直接插入节点*/
    if (tree->root == tree->sentinel) {
        node->parent = NULL;
        node->left = tree->sentinel;
        node->right = tree->sentinel;
        tree->root = node;
        rbt_set_color(node, BLACK); 
        return;
    }

    /* 二叉树插入节点 */
    rbtree_node_t *cur = tree->root;
    rbtree_node_t **p;
    while (true) {
        p = tree->ops.cmp(node->data, cur->data) < 0 ? &cur->left : &cur->right;
        if (*p == tree->sentinel) {
            break;
        }
        cur = *p;
    }
    node->left = tree->sentinel;
    node->right = tree->sentinel;
    node->parent = cur;
    *p = node;
    rbt_set_color(node, RED);
}

/**
 * 右旋示例如下, n代表node, t代表tmp.
 *       n                t
 *      / \              / \
 *     t   a3    --->   a1  n
 *    / \                  / \
 *   a1  a2               a2  a3
 */
static void rbtree_right_rotate(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_node_t *tmp = node->left;
    node->left = tmp->right;
    if (tmp->right != tree->sentinel) {
        tmp->right->parent = node;
    }

    tmp->parent = node->parent;
    if (node == tree->root) {
        tree->root = tmp;
    } else if (node == node->parent->left){
        node->parent->left = node;
    } else {
        node->parent->right = node;
    }

    tmp->right = node;
    node->parent = tmp;
    return;
}

/**
 * 左旋示例如下, n代表node, t代表tmp.
 *      n                 t
 *     / \               / \
 *    a1  t    --->     n   a3
 *       / \           / \
 *      a2  a3        a1  a2
 */
static void rbtree_left_rotate(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_node_t *tmp = node->right;
    node->right = tmp->left;
    if (tmp->left != tree->sentinel) {
        tmp->left->parent = node;
    }

    tmp->parent = node->parent;

    if (node == tree->root) {
        tree->root = tmp;
    } else if (node == node->parent->left) {
        node->parent->left = tmp;
    } else {
        node->parent->right = tmp;
    }

    tmp->left = node;
    node->parent = tmp;
}

static rbtree_node_t *rbt_create_node(rbtree_t *tree, void *data)
{
    rbtree_node_t *node = (rbtree_node_t *)malloc(sizeof(*node));
    if (node == NULL) {
        diag_err("malloc memory for node failed");
        return NULL;
    }
    node->color = RED;
    node->data = tree->ops.dup(data);
    return node;
}

int rbtree_insert(rbtree_t *tree, void *data)
{
    rbtree_node_t *node = rbt_create_node(tree, data);

    /* 节点直接插入树 */
    rbtree_insert_node(tree, node);
    tree->num++;
    /* 红黑树树自旋转进行平衡 */
    while ((node != tree->root) && (rbt_is_red(node->parent))) {
        rbtree_node_t *grandparent = node->parent->parent;
        if (node->parent == grandparent->left) {
            rbtree_node_t *uncle = grandparent->right;
            if (rbt_is_red(uncle)) {
                /**
                *   case1: 重染色，大写代表黑色，小写代表红色。
                *
                *        G              g
                *       / \            / \
                *      p   u  --->    P   U
                *     /              /
                *    x              x
                */
                rbt_set_color(node->parent, BLACK);
                rbt_set_color(uncle, BLACK);
                rbt_set_color(grandparent, RED);
                node = grandparent;
                continue;
            }
            /* 旋转 */
            if (node == node->parent->right) {
                /**
                 * case2: 左旋，大写代表黑色，小写代表红色。
                 *
                 *        G              g
                 *       / \            / \
                 *      p   u  --->    x   U
                 *       \            /
                 *        x          p
                 */
                node = node->parent;
                rbtree_left_rotate(tree, node);
            }
            /**
             *  case3: 右旋，大写代表黑色，小写代表红色。
             *
             *        G              P
             *       / \            / \
             *      p   U  --->    x   g
             *     /                    \
             *    x                      U
             */
            rbt_set_color(node->parent, BLACK);
            rbt_set_color(node->parent->parent, RED);
            rbtree_right_rotate(tree, node->parent->parent);
        } else {
            rbtree_node_t *uncle = grandparent->left;
            if (rbt_is_red(uncle)) {
                /* 重染色*/
                rbt_set_color(node->parent, BLACK);
                rbt_set_color(uncle, BLACK);
                rbt_set_color(grandparent, RED);
                node = grandparent;
                continue;
            }
            /* 旋转自平衡 */
            if (node == node->left) {
                node = node->parent;
                rbtree_right_rotate(tree, node);
            }
            rbt_set_color(node->parent, BLACK);
            rbt_set_color(node->parent->parent, RED);
            rbtree_left_rotate(tree, node->parent->parent);
        }
    }
    rbt_set_color(tree->root, BLACK);
    return 0;
}

rbtree_node_t *rbtree_find(rbtree_t *tree, void *data)
{
    rbtree_node_t *tmp = tree->root;
    while (tmp != tree->sentinel) {
        int ret = tree->ops.cmp(data, tmp->data);
        if (ret == 0) {
            return tmp;
        } else if (ret < 0) {
            tmp = tmp->left;
        } else {
            tmp = tmp->right;
        }
    }
    return NULL;
}

rbtree_node_t *rbtree_min_node(rbtree_t *tree, rbtree_node_t *node)
{
    while (node->left != tree->sentinel) {
        node = node->left;
    }
    return node;
}

static bool is_inner_node(rbtree_t *tree, rbtree_node_t *node)
{
    if ((node->left != tree->sentinel) && (node->right != tree->sentinel)) {
        return true;
    }
    return false;
}

static void rbtree_node_free(rbtree_node_t *node)
{
    free(node->data);
    free(node);
}

rbtree_node_t *rbtree_delete_node(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_node_t *temp = NULL;
    rbtree_node_t *subst = NULL;
    if (node->left == tree->sentinel) {
        subst = node;
        temp = node->right;
    } else if (node->right == tree->sentinel) {
        subst = node;
        temp = node->left;
    } else {
        subst = rbtree_min_node(tree, node->right);
        temp = subst->right;
    }

    if (subst == tree->root) {
        tree->root = temp;
        rbt_set_color(temp, BLACK);
        rbtree_node_free(subst);
        return NULL;
    }

    uint8_t color = subst->color;
    if (subst == subst->parent->left) {
        subst->parent->left = temp;
    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {
        temp->parent = subst->parent;
    } else {
        if (subst->parent == node) {
            temp->parent = subst;
        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        subst->color = node->color;

        if (node == tree->root) {
            tree->root = subst;
        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }
        if (subst->left != tree->sentinel) {
            subst->left->parent = subst;
        }
        if (subst->right != tree->sentinel) {
            subst->right->parent = subst;
        }
    }
    rbtree_node_free(node);
    if (color == RED) {
        return NULL;
    }
    return temp;
}

void rbtree_delete_rebalence(rbtree_t *tree, rbtree_node_t *tmp)
{
    /* fix red black tree*/
    while ((tmp != tree->root) && (!rbt_is_red(tmp))) {
        if (tmp == tmp->parent->left) {
            rbtree_node_t *sibling = tmp->parent->right;
            if (rbt_is_red(sibling)) {
                rbt_set_color(sibling, BLACK);
                rbt_set_color(tmp->parent, RED);
                rbtree_left_rotate(tree, tmp->parent);
                sibling = tmp->parent->right;
            }
            if (!rbt_is_red(sibling->left) && !rbt_is_red(sibling->right)) {
                rbt_set_color(sibling, RED);
                tmp = tmp->parent;
            } else {
                if (!rbt_is_red(sibling->right)) {
                    rbt_set_color(sibling, RED);
                    rbt_set_color(sibling->left, BLACK);
                    rbtree_right_rotate(tree, sibling);
                    sibling = tmp->parent->right;
                }
                sibling->color = tmp->parent->color;
                rbt_set_color(sibling->right, BLACK);
                rbt_set_color(tmp->parent, BLACK);
                rbtree_left_rotate(tree, tmp->parent);
                tmp = tree->root;
            }
        } else {
            rbtree_node_t *sibling = tmp->parent->left;
            if (rbt_is_red(sibling)) {
                rbt_set_color(sibling, BLACK);
                rbt_set_color(tmp->parent, RED);
                rbtree_right_rotate(tree, tmp->parent);
                sibling = tmp->parent->left;
            }
            if (!rbt_is_red(sibling->left) && !rbt_is_red(sibling->right)) {
                rbt_set_color(sibling, RED);
                tmp = tmp->parent;
            } else {
                if (!rbt_is_red(sibling->left)) {
                    rbt_set_color(sibling, RED);
                    rbt_set_color(sibling->right, BLACK);
                    rbtree_left_rotate(tree, sibling);
                    sibling = tmp->parent->left;
                }
                sibling->color = tmp->parent->color;
                rbt_set_color(sibling->left, BLACK);
                rbt_set_color(tmp->parent, BLACK);
                rbtree_right_rotate(tree, tmp->parent);
                tmp = tree->root;
            }
        }
    }
    rbt_set_color(tmp, BLACK);
}

int rbtree_delete(rbtree_t *tree, void *data)
{
    rbtree_node_t *node = rbtree_find(tree, data);
    if (node == NULL) {
        return -1;
    }
    tree->num--;
    rbtree_node_t *tmp = rbtree_delete_node(tree, node);
    if (tmp) {
        rbtree_delete_rebalence(tree, tmp);
    }
    return 0;
}

void rbtree_dump(rbtree_t *tree, rbtree_node_t *node, int depth)
{
    if (node == tree->sentinel) {
        return;
    }
    const char *str = tree->ops.dump(node->data);
    const char *color = node->color == RED ? "r" : "b";
    char space_str[128] = {0};
    for (int i = 0; i < depth; i++) {
        space_str[i] = ' ';
    }
    diag_info("%s%s%s", space_str, str, color);

    rbtree_dump(tree, node->left, depth + 1);
    rbtree_dump(tree, node->right, depth + 1);
}