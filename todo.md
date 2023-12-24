### 一、学习内容

知名开源学习： redis、nginx，rocksdb

演进方向：

| 大方向   | 内容                       | 开源学习                                                   | 状态                                     |
| -------- | -------------------------- | ---------------------------------------------------------- | ---------------------------------------- |
| 工程技术 | mkfile/cmake/configuration | NA                                                         | OK                                       |
|          | llt框架                    | [Unity](https://github.com/ThrowTheSwitch/Unity)           | unittest.h                               |
| 数据结构 | list                       | [redis](https://github.com/redis/redis)                    | OK                                       |
|          | rbtree                     | [nginx](https://github.com/nginx/nginx)                    | rbtree.h                                 |
|          | 动态数组                   | [redis](https://github.com/redis/redis)                    | sds.h                                    |
| 序列化   | 序列化                     | protobuf                                                   | NOK                                      |
| 协程框架 | 协程框架                   |                                                            | NOK                                      |
| 实用组件 | 线程池                     | [C-Thread-Pool](https://github.com/Pithikos/C-Thread-Pool) | thread_pool.h                            |
|          | ringbuf                    |                                                            | ringbuff.h                               |
|          | 内存池                     |                                                            | NOK                                      |
|          | 消息队列                   |                                                            | NOK                                      |
|          | 定时器                     |                                                            | NOK                                      |
|          | 协程                       |                                                            | NOK                                      |
| 协议栈   | 用户态协议栈               | NtyTcp                                                     | NOK                                      |
| 压缩算法 | lz77                       | [FastLz](https://github.com/ariya/FastLZ)                  | NOK                                      |
|          | huffman                    | [huffman](https://github.com/drichardson/huffman)          | [huffman.h](src/core/compress/huffman.h) |
| http     | url解析                    |                                                            | url.h                                    |
|          | http请求                   |                                                            | OK                                       |
| 配置解析 | xml or json                |                                                            | NOK                                      |
|          | protobuf                   |                                                            |                                          |



### 二、待完成计划

- 最简单的压缩算法。
  - lz77
  - huffman
  - 日志压缩
  
  
