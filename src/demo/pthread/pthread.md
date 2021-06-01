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
```
```