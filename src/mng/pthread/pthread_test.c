#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define THREAD1_RUN_TIMES 8
#define THREAD_NUM 2

pthread_t g_thread_id[THREAD_NUM];
pthread_cond_t g_cond;
pthread_mutex_t g_mutex;

void *thread_run1(void *val)
{
    int count = 0;
    while (1) {
        printf("thread1 run~~~, count%d\n", count);
        pthread_mutex_lock(&g_mutex);
        pthread_cond_wait(&g_cond, &g_mutex);
        pthread_mutex_unlock(&g_mutex);
        count++;
    }
    return NULL;
}

void *thread_run2(void *val)
{
    int count = 0;
    while (1) {
        printf("thread2 run~~~, count%d\n", count);
        pthread_mutex_lock(&g_mutex);
        pthread_cond_wait(&g_cond, &g_mutex);
        pthread_mutex_unlock(&g_mutex);
        count++;
    }
    return NULL;
}

int main()
{
    pthread_create(&g_thread_id[0], NULL, thread_run1, NULL);
    pthread_detach(g_thread_id[0]);

    pthread_create(&g_thread_id[1], NULL, thread_run2, NULL);
    pthread_detach(g_thread_id[1]);

    while (1) {
        pthread_cond_signal(&g_cond);
        sleep(2);
    }
    return 0;
}