## pthread相关知识
### 1、常用库函数

1.1 线程创建  <br>
```
/*
@return 0表示成功, 非0 表示失败
*/
int pthread_create(pthread_t *th, const pthread_attr_t *attr, void *(* func)(void *), void *arg);
```
1.2 线程退出 <br>
其他线程使用phtread_cancel终止线程 <br>
```
int pthread_cancel (pthread_t thread)
```
本线程使用pthread_exit终止线程<br>
```
void pthread_exit (void *retval)
```
1.3 线程的阻塞 <br>
阻塞是线程间同步的方法
```
int pthread_join(pthread_t threadid, void **value_ptr)
```