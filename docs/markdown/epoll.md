1、Socket编程

```c
/* 创建一个通信的端点 */
int socket(int domain, int type, int protocol);

/* 绑定一个名字到socket上*/
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/* 监听在一个socket上的连接 */
int listen(int sockfd, int backlog);

/* 接受一个socket上的连接*/
int accept(int sockfd,struct sockaddr *addr, socklen_t *addrlen,int flags);
```

2、epoll 多路复用

