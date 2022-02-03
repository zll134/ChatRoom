/* ********************************
 * Author:       Zhanglele
 * Description:  线程池管理组件
 * Create time:  2021.09.21
 ********************************/
#include "thread_pool.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/prctl.h>

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
    struct job *prev; /* 表示任务节点指向的前一个节点 */
    thread_op op;
    void *args;
} job_t;

/* 任务队列 */
typedef struct job_queue {
    pthread_mutex_t push_mutex; /* 入任务队列的锁 */
    pthread_mutex_t pop_mutex; /* 出任务队列的锁 */
    job_t front;
    job_t tail;
    int len;
    bsem_t has_jobs;
} job_queue_t;

typedef struct thread {
    pthread_t id;
    uint32_t index;
    thread_pool_t *pool;
} thread_t;

typedef struct thread_pool {
    thread_t *threads;
    uint32_t worker_num;
    uint32_t threads_num;
    job_queue_t queue;
} thread_pool_t;

static void bsem_init(bsem_t *sem)
{
    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->cond, NULL);
    sem->v = 0;
}

static void bsem_wait(bsem_t *sem)
{
    pthread_mutex_lock(&sem->mutex);
    if (sem->v == 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    pthread_mutex_unlock(&sem->mutex);
}

static void bsem_post(bsem_t *sem)
{
    sem->v = 1;
    pthread_cond_signal(&sem->cond);
}

static void job_queue_init(job_queue_t *queue)
{
    pthread_mutex_init(&queue->push_mutex, NULL);
    pthread_mutex_init(&queue->pop_mutex, NULL);
    queue->front = (job_t) {0};
    queue->tail = (job_t) {0};
    queue->front.prev = &queue->tail;
    queue->tail.next = &queue->front;
    queue->len = 0;
    bsem_init(&queue->has_jobs);
}

static void job_queue_push(job_queue_t *queue, job_t *job)
{
    pthread_mutex_lock(&queue->push_mutex);
    job_t *next = queue->tail.next;
    job->next = next;
    job->prev = &queue->tail;
    queue->tail.next = job;
    next->prev = job;
    queue->len++;
    bsem_post(&queue->has_jobs);
    pthread_mutex_unlock(&queue->push_mutex);
}

static job_t *job_queue_pop(job_queue_t *queue)
{
    pthread_mutex_lock(&queue->pop_mutex);
    if (queue->len <= 0) {
        printf("job queue is empty\n");
        return NULL;
    }

    job_t *new_job = queue->front.prev;
    job_t *prev = new_job->prev;
    prev->next = &queue->front;
    queue->front.prev = prev;
    queue->len--;
    if (queue->len == 0) {
        bsem_init(&queue->has_jobs);
    }
    pthread_mutex_unlock(&queue->pop_mutex);
    return new_job;
}

void *thread_do(void *args)
{
    thread_t *thread = (thread_t *)args;
    thread_pool_t *pool = thread->pool;
    char thread_name[32] = {0};
    sprintf(thread_name, "pool-thread-%d", thread->index);
    prctl(PR_SET_NAME, thread_name);

    while (true) {
        printf("thread start do in loop, index: %d\n", thread->index);
        bsem_t *has_jobs = &pool->queue.has_jobs;
        bsem_wait(has_jobs);

        pool->worker_num++;
        job_t *job = job_queue_pop(&pool->queue);
        job->op(job->args);
        pool->worker_num--;
        free(job);
    }
}

static void threads_init(thread_pool_t *pool, uint32_t threads_num)
{
    pool->threads = (thread_t *)malloc(threads_num * sizeof(thread_t));
    if (pool->threads == NULL) {
        printf("create threads failed");
        return;
    }

    for (int i = 0; i < threads_num; i++) {
        thread_t *thread = &pool->threads[i];
        thread->pool = pool;
        thread->index = i;
        printf("create thread");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, thread_do, thread);
        pthread_detach(thread_id);
    }
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

    /* 初始化多线程 */
    threads_init(pool, threads_num);

    /* 任务队列初始化 */
    job_queue_init(&pool->queue);
    return pool;
}

void thread_pool_add_work(thread_pool_t *pool, thread_op op, void *args)
{
    job_t *job = (job_t *)malloc(sizeof(job_t));
    if (job == NULL) {
        return;
    }
    job->next = NULL;
    job->op = op;
    job->args = args;
    job_queue_push(&pool->queue, job);
}

void thread_pool_destroy(thread_pool_t *pool)
{
    free(pool->threads);
    free(pool);
    return;
}

uint32_t thread_pool_get_worker_num(thread_pool_t *pool)
{
    return pool->threads_num;
}