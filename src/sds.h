/* ********************************
 * Author:       Zhanglele
 * Description:  简单动态数组的实现
 * create time: 2022.01.16
 ********************************/

#ifndef SDS_H
#define SDS_H

#include <stdint.h>

typedef char * sds_t;

typedef struct {
    uint32_t len;
    uint32_t free;
    char buf[];
} sds_hdr_t;

sds_t sds_new(const char *init);
void sds_free(sds_t obj);

#endif