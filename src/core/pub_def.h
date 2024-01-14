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
#define MAX_UINT8 0xff

/* 通用错误码定义 */
#define TOY_OK 0      // 成功返回码
#define TOY_ERR (-1)  // 通用错误码

/* 100~199 为哈希模块错误码 */
#define TOY_ERR_DICT_MODULE_BASE                100

/* 200~299 为压缩模块的错误码 */
#define TOY_ERR_LZ_LEVEL_INVALID                201
#define TOY_ERR_LZ_DICT_CREATE_FAIL             202
#define TOY_ERR_LZ_INVALID_PARA                 203
#define TOY_ERR_LZ_OUT_MEM_UNSUFFICIENT         204
#define TOY_ERR_LZ_BACKWARD_NOT_EXIST           205
#define TOY_ERR_LZ_RINGBUF_CREATE_FAIL          206
#define TOY_ERR_LZ_CREATE_DICT_FAIL             207

/* 300~399 为ringbuff的错误码 */
#define TOY_ERR_RINGBUFF_WRITE_INVALID_PARA     301
#define TOY_ERR_RINGBUFF_READ_INVALID_PARA      302


/* 400~499 为huffman的错误码 */
#define TOY_ERR_HUFFMAN_MODULE_BASE             400

/* 500~599 为stream的错误码 */
#define TOY_ERR_STREAM_MODULE_BASE              500

/* 600~699 为stream的错误码 */
#define TOY_ERR_BIG_INT_MODULE_BASE             600

#endif