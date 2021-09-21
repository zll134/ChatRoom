/* ********************************
 * Author:       Zhanglele
 * Description:  线程池管理组件
 ********************************/
#include "thread_pool.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS_MAX_NUM 20

/* 二值信号量 */
typedef struct bsem {
   pthread_mutex_t mutex;
   pthread_cond_t cond;
   int v; 
} bsem_t;

/* 任务 */
typedef struct job {
    struct job *next; /* 表示该任务节点指向的下一个节点 */
    thread_op op;
    void *arg;
} job_t;

/* 任务队列 */
typedef struct job_queue {
    pthread_mutex_t push_mutex; /* 入任务队列的锁 */
    pthread_mutex_t pop_mutex; /* 出任务队列的锁 */
    job_t *front;
    job_t *rear;
    int len;
    bsem_t has_jobs;
} job_queue_t;

typedef struct thread {
    pthread_t id;
    thread_pool_t *pool;
} thread_t;

typedef struct thread_pool {
    thread_t *threads;
    uint32_t worker_num;
    uint32_t threads_num;
    job_queue_t queue;
} thread_pool_t;

void job_queue_init(job_queue_t *que)
{
    return;
}

void thread_init(thread_pool_t *pool)
{
    return;
}

thread_pool_t *thread_pool_init(uint32_t threads_num)
{
    thread_pool_t *pool = (thread_pool_t *)malloc(sizeof(*pool));
    if (pool == NULL) {
        printf("create thread pool failed");
        return NULL;
    }

    *pool = (thread_pool_t) {0};
    pool->threads_num = threads_num;

    for (int i = 0; i < threads_num; i++) {
        thread_init(pool);
    }
    /* 任务队列初始化 */
    job_queue_init(&pool->queue);
    return pool;
}