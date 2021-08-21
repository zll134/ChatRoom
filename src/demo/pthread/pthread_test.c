#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define THREAD1_RUN_TIMES 8
#define THREAD_NUM 2

pthread_t g_thread_id[THREAD_NUM];

void *thread_run(void *val)
{
    pthread_t tid = *(pthread_t *)val;
    int index = 0;
    while (1) {
        printf("thread%d run~~~, count:%d\n", tid, index + 1);
        sleep(1);
        index++;
    }   
    return NULL;
}
int main()
{
    for (int i = 0; i < THREAD_NUM; i++) {
        int ret = pthread_create(&g_thread_id[i], NULL, thread_run, "1");
        if (ret != 0) {
         printf("%s", "create pthread failed\n");
    }
    signal(SIGALRM, signal_func);
    alarm(10);

    while (1) {
        ;
    }
    return 0;
}