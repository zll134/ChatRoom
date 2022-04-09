/* ********************************
 * Author:       Zhanglele
 * Description:  客户端和服务端之间消息交互
 * create time: 2022.4.5
 ********************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include "pub.h"

enum {
    MSG_LOGIN,
    MSG_CHAT
};

enum {
    LOGIN_SUCCESS,
    LOGIN_USER_EXIST
};

typedef struct {
    uint8_t type;
    uint32_t len;
} msg_hdr_t;

typedef struct {
    char name[USER_NAME_LEN];
} msg_login_req_t;

typedef struct {
    char result;
} msg_login_resp_t;

typedef struct {
    char name[USER_NAME_LEN];
    char buf[CHAT_BUF_LEN];
} msg_chat_req_t;

int msg_send(int fd, uint8_t type, void *data, uint32_t len);

#endif