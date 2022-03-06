/* ********************************
 * Author:       Zhanglele
 * Description:  线程池管理组件
 * create time: 2021.09.21
 ********************************/

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdint.h>

typedef struct thread_pool thread_pool_t;

typedef void *(*thread_op)(void *arg);

/* 线程池初始化 */
thread_pool_t *thread_pool_init(uint32_t threads_num);

/* 线程池添加任务 */
void thread_pool_add_work(thread_pool_t *pool, thread_op op, void *args);

/* 销毁线程池 */
void thread_pool_destroy(thread_pool_t *pool);

/* 获取线程池内线程数量 */
uint32_t thread_pool_get_worker_num(thread_pool_t *pool);

#endif