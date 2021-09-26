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
    void *args;
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

static void bsem_init(bsem_t *sem)
{
    pthread_mutex_init(&sem.mutex, NULL);
    pthread_cond_init(&sem.cond, NULL);
    queue->has_jobs.v = 0;
}

static void bsem_wait(bsem_t *sem)
{
    pthread_mutex_lock(sem->mutex);
    if (sem->v == 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    pthread_mutex_unlock(sem->mutex);
}

static void bsem_post(bsem_t *sem)
{
    sem->v = 1;
    pthread_cond_signal(&sem->cond);
}

static void job_queue_init(job_queue_t *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->len = 0;
    pthread_mutex_init(&queue->push_mutex, NULL);
    pthread_mutex_init(&queue->pop_mutex, NULL);

    return;
}

static void job_queue_push(job_queue_t *queue, job_t *job)
{
    pthread_mutex_lock(&queue->push_mutex);
    job->next = queue->rear;
    queue->rear = job;
    queue->len++;
    bsem_post(&queue->has_jobs);
    pthread_mutex_unlock(&queue->push_mutex);
}

static int job_queue_pop(job_queue_t *queue, job_t *job)
{
    pthread_mutex_lock(&queue->pop_mutex);
    if (queue->len > 0) {

    }
    job_t *old_job = queue->front;
    pthread_mutex_unlock(&queue->pop_mutex);
}

void *thread_do(void *args)
{
    thread_t *thread = (thread_t *)args;
    thread_pool_t *pool = thread->pool;
    while (true) {
        bsem_t *has_jobs = &pool->queue.has_jobs;
        bsem_wait(has_jobs);

        pool->worker_num++;
        job_t job = {0};
        job_queue_pop(&pool->queue, &job);
        job.op(job.args);
        pool->worker_num--;
    }
}

static void thread_init(thread_t *thread)
{
    pthrad_t thread_id;
    pthread_create(&thread_id, NULL, thread_do, thread);
    pthread_detach(thread_id);
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

    pool->threads = (thread_t *)malloc(threads_num * sizeof(thread_t));
    if (pool->threads == NULL) {
        printf("create threads failed");
        return NULL;
    }
    for (int i = 0; i < threads_num; i++) {
        pool->threads[i].pool = pool;
        thread_init(&pool->threads[i]);
    }
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

void thread_pool_wait(void)
{
    return;
}

void thread_pool_destroy(thread_pool_t *pool)
{
    return;
}