/* ********************************
 * Author:       Zhanglele
 * Description:  客户端和服务端之间消息交互
 * create time: 2022.4.5
 ********************************/
#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"
#include "pub_def.h"

int msg_send(int fd, uint8_t type, void *data, uint32_t len)
{
    if ((data == NULL) || (len == 0)) {
        diag_err("input data param is invalid");
        return -1;
    }
    diag_info("send msg with type %u", type);
    void *msg = (void *)malloc(sizeof(msg_hdr_t) + len);
    if (msg == NULL) {
        return -1;
    }
    msg_hdr_t *hdr = (msg_hdr_t *)msg;
    hdr->type = type;
    hdr->len = len;
    memcpy(hdr + 1, data, len);

    if (write(fd, msg, sizeof(msg_hdr_t) + len) == -1) {
        free(msg);
        diag_err("send msg failed, fd %d. reason %s.", fd, strerror(errno));
        return -1;
    }
    free(msg);
    return 0;
}