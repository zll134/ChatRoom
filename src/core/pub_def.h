/* ********************************
 * Author:       Zhanglele
 * Description:  公共头文件
 * create time: 2022.4.5
 ********************************/
#ifndef PUB_DEF_H
#define PUB_DEF_H

#define USER_NAME_LEN 32

#define SERVER_PORT 8089

#define CHAT_BUF_LEN 512

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define LONG_MAX 0x7fffffff

/* 通用错误码定义 */
#define TOY_OK 0      // 成功返回码
#define TOY_ERR (-1)  // 通用错误码

/* 101~200 为哈希模块错误码 */
#define TOY_ERR_DICT_MALLOC_FAILED     101
#define TOY_ERR_INVALID_REHARSH        102

#endif