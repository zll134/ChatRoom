## pthread相关知识
#### 1、线程管理

```c
/* 线程创建函数 */
int pthread_create(pthread_t *th, const pthread_attr_t *attr, void *(* func)(void *), void *arg);

/* 其他线程终止本线程 */
int pthread_cancel (pthread_t thread)
void pthread_exit (void *retval)

/* 线程同步，以阻塞的方式等待threadid的线程资源被挥手 */
int pthread_join(pthread_t threadid, void **value_ptr)
 
/* 线程分离，主线程与子线程分离，退出时直接回收资源 */
int pthread_detach(pthread_t t);
```

#### 2、条件变量

```c
/* 条件变量的初始化 */
int pthread_cond_init(pthread_cond_t *cv, const pthread_condattr_t *a);

/* 发信号唤醒休眠的线程, signal一次只能唤醒一个，broadcast一次唤醒所有 */
int pthread_cond_signal(pthread_cond_t *cv);
int pthread_cond_broadcast (pthread_cond_t *cv);

/* 阻塞休眠当前线程， 等待signal唤醒 */
int pthread_cond_wait (pthread_cond_t *cv, pthread_mutex_t *external_mutex);
int pthread_cond_timedwait(pthread_cond_t *cv, pthread_mutex_t *external_mutex, const struct timespec *t);

/* 条件变量删除 */
int pthread_cond_destroy(pthread_cond_t *cv);
```

