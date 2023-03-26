/* ********************************
 * Author:       Zhanglele
 * Description:  http 公共宏定义模块
 * create time: 2023.01.27
 ********************************/
#ifndef HTTP_PUB_H
#define HTTP_PUB_H

typedef enum {
    HTTP_GET,
    HTTP_PUT,
    HTTP_HEAD,
    HTTP_DELETE,
    HTTP_INVALID_METHOD,
} HTTP_METHOD_TYPE_E;

#endif