#include <stdio.h>
#include <stdlib.h>
#include "thread_pool.h"
#include <pthread.h>
#include "connect.h"

#define DEFAULT_THREAD_NUM 10

pthread_mutex_t g_mutext;
int g_atomic_sum = 0;
void *test_sum_atomic(void *param)
{
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&g_mutext);
        g_atomic_sum++;
        pthread_mutex_unlock(&g_mutext);
        printf("g_atomic_sum %d\n", g_atomic_sum);
    }
}
void test_atomic(thread_pool_t *pool)
{
    pthread_mutex_init(&g_mutext, NULL);
    int atomic_sum = 0;
    int num = thread_pool_get_worker_num(pool);
    for (int i = 0; i < num; i++) {
        thread_pool_add_work(pool, test_sum_atomic, NULL);
    }
}

int main()
{
    thread_pool_t *pool = thread_pool_init(DEFAULT_THREAD_NUM);
    test_atomic(pool);
    connect_init();
    return 0;
}