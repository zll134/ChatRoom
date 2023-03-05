/* ********************************
 * Author:       Zhanglele
 * Description:  http 请求管理
 * create time: 2023.02.05
 ********************************/
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "sds.h"
#include "t_list.h"
#include "http_pub.h"

int http_request(HTTP_METHOD_TYPE_E method, url_data_t *url_data);

#endif