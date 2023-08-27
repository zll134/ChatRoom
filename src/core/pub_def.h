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
#define TOY_ERR_DICT_MALLOC_FAILED              101
#define TOY_ERR_DICT_INVALID_PARA               102
#define TOY_ERR_DICT_IS_REHASHING               103
#define TOY_ERR_DICT_CONFIG_NULL                104
#define TOY_ERR_DICT_CALLBACK_NULL              105
#define TOY_ERR_DICT_PARAM_INVALID              106
#define TOY_ERR_DICT_KEY_EXIST                  107
#define TOY_ERR_DICT_KEY_NOT_EXIST              108
#define TOY_ERR_DICT_INVALID_REALSIZE           109

/* 201~300 为压缩模块的错误码 */
#define TOY_ERR_LZ_LEVEL_INVALID                201
#define TOY_ERR_LZ_DICT_CREATE_FAIL             202
#define TOY_ERR_LZ_INVALID_PARA                 203
#define TOY_ERR_LZ_OUT_MEM_UNSUFFICIENT         204
#define TOY_ERR_LZ_BACKWARD_NOT_EXIST           205
#define TOY_ERR_LZ_RINGBUF_CREATE_FAIL          206

/* 201~300 为ringbuff的错误码 */
#define TOY_ERR_RINGBUFF_WRITE_INVALID_PARA     301
#define TOY_ERR_RINGBUFF_READ_INVALID_PARA      302
\
#endif