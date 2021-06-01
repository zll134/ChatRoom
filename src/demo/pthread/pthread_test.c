#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define THREAD1_RUN_TIMES 8
void *thread1_run(void *val)
{
    for (int i = 0; i < THREAD1_RUN_TIMES; i++) {
        printf("thread%s run~~~, count:%d\n", (char *)val, i + 1);
        sleep(1);
    }
    return NULL;
}
int main()
{
    printf("-----");
    pthread_t tid1 = 0;
    int ret = pthread_create(&tid1, NULL, thread1_run, "1");
    if (ret != 0) {
         printf("%s", "create pthread failed\n");
    }
    int count = 0;
    while (1) {
         printf("main thread run~, count:%d\n", count);
         sleep(1);
         count++;
         if (count > 10) {
             break;
         }
    }
    return 0;
}