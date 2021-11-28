### 一、Socket编程

```c
/* 创建一个通信的端点 */
int socket(int domain, int type, int protocol);

/* 绑定一个名字到socket上*/
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/* 监听在一个socket上的连接 */
int listen(int sockfd, int backlog);

/* 接受一个socket上的连接 */
int accept(int sockfd,struct sockaddr *addr, socklen_t *addrlen,int flags);
```

### 二、epoll 多路复用

#### 2.1  epoll_create

返回epoll的文件描述符。size参数在最新的内核中可以忽略掉了。

```c
/* 创建epoll_create文件描述符 */
int epoll_create(int size)；
```

#### 2.2 epoll_ctl

对epoll 内部的文件描述符进行操作

```c
/* 对于epoll文件描述符的控制*/
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)；
```

op的类型

```c
EPOLL_CTL_ADD  添加事件
EPOLL_CTL_MOD  修改事件
EPOLL_CTL_DEL  删除事件
```

epoll事件的结构体定义。成员events是一个mask。

```c
typedef union epoll_data {
    void      *ptr;
    int       fd;
    uint32_t  u32;
    uint64_t  u64;
} epoll_data_t;
struct epoll_event {
    uint32_t     events;  /* Epoll events */
    epoll_data_t data;    /* User data variable */
};

/* events类型 */
EPOLLIN  对应read操作
EPOLLOUT 对应write操作
    
```

#### 2.3 epoll_wait 

```c
/* 等待io时间来临 */
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
```

