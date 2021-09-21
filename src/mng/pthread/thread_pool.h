/* ********************************
 * Author:       Zhanglele
 * Description:  线程池管理组件
 ********************************/

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdint.h>

typedef struct thread_pool thread_pool_t;

typedef void *(*thread_op)(void *arg);

/* 线程池初始化 */
thread_pool_t *thread_pool_init(uint32_t threads_num);

/* 线程池添加任务 */
void thread_pool_add_work(thread_op op, void *args);

/* 线程池等待任务执行完毕 */
void thread_pool_wait(void);

/* 销毁线程池 */
void thread_pool_destroy(thread_pool_t *pool);

#endif