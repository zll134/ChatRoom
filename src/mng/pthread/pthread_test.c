#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "thread_pool.h"

#define THREAD_NUM 5
#define JOB_NUM 100
int g_job_index[JOB_NUM] = {0};

void *thread_run(void *val)
{
    int index = *(int *)val;
    printf("thread start run, index is %d\n", index);
    sleep(1);
    return NULL;
}

int main()
{
    thread_pool_t *pool = thread_pool_init(THREAD_NUM);
    printf("create thread pool success\n");
    for (int i = 0; i < JOB_NUM; i++) {
        g_job_index[i] = i;
        thread_pool_add_work(pool, thread_run, &g_job_index[i]);
    }

    printf("add work to thread pool success\n");
    while (1) {
        sleep(2);
    }
    return 0;
}