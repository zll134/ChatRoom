/* ********************************
 * Author:       Zhanglele
 * Description:  简单动态数组的实现
 * create time: 2022.01.16
 ********************************/

#include "sds.h"

#include <stdlib.h>
#include <string.h>

sds_t sds_new(const char *init)
{
    int len = strlen(init);
    sds_hdr_t *sh = NULL;
    
    sh = (sds_hdr_t *)malloc(sizeof(sds_hdr_t) + len + 1);
    if (sh == NULL) {
        return NULL;
    }
    sh->free = 0;
    sh->len = len;
    memcpy(sh->buf, init, len);
    return sh->buf;
}

void sds_free(sds_t obj)
{
    sds_hdr_t *sh = (sds_hdr_t *)(obj - sizeof(sds_hdr_t));
    free(sh);
}