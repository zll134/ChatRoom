/* ********************************
 * Author:       Zhanglele
 * Description:  简单动态数组的实现
 * create time: 2022.01.16
 ********************************/

#include "sds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

#define MEMORY_THRESHOLD (2 * 1024 * 1024)

/* sds的结构：
    | str len | free len | string        |
*/

sds_t sds_new_with_len(const char *init, int len)
{
    sds_hdr_t *sh = (sds_hdr_t *)malloc(sizeof(sds_hdr_t) + len + 1);
    if (sh == NULL) {
        return NULL;
    }
    sh->free = 0;
    sh->len = len;
    if (init != NULL) {
        memcpy(sh->buf, init, len);
    } else {
        memset(sh->buf, 0, len);
    }

    sh->buf[sh->len] = '\0';
    return sh->buf;
}

sds_t sds_new(const char *init)
{
    int len = strlen(init);
    return sds_new_with_len(init, len);
}

void sds_free(sds_t obj)
{
    sds_hdr_t *sh = (sds_hdr_t *)(obj - sizeof(sds_hdr_t));
    free(sh);
}

const char *sds_get_string(sds_t obj)
{
    return (const char *)obj;
}

uint32_t sds_get_len(sds_t obj)
{
    if (obj == NULL) {
        return 0;
    }
    sds_hdr_t *sh = (sds_hdr_t *)(obj - sizeof(sds_hdr_t));
    return sh->len;
}

int sds_find_str(sds_t obj, uint32_t start, uint32_t end, const char *str)
{
    uint32_t len = sds_get_len(obj);
    int str_len = strlen(str);
    if ((start >= end) || (end > len) || (str_len > (end - start))) {
        diag_err("[SDS]Input param is invalid, objcontent %s, start %u, end %u,"
                 " obj len %u, str %s, str_len %d.",
                 (char *)obj, start, end, len, str, str_len);
        return -1;
    }

    const char *obj_str = sds_get_string(obj);
    for (int i = start; i < end - str_len; i++) {
        if (strncmp(obj_str + i, str, str_len) == 0) {
            return i;
        }
    }

    return -1;
}

sds_t sds_substr(sds_t src, int start, int end)
{
    if ((src == NULL) || (start >= end) || (end > sds_get_len(src))) {
        diag_err("[SDS]Input param is invalid when getting substr, src %s, start %u, end %u,",
                 (char *)src, start, end);
        return NULL;
    }
    uint32_t src_len = sds_get_len(src);
    
    sds_t substr = sds_new_with_len(NULL, end - start);
    if (substr == NULL) {
        diag_err("Create substr failed, src %s, start %d, end %d.", (char *)src, start, end);
        return NULL;
    }
    (void)memcpy(substr, src + start, end - start);
    return substr;
}