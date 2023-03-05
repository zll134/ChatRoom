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
    |---  sds_hdr_t ---|------     buf  -----------|
    | strlen | freelen |   str    | \0 |   free    |
*/

sds_t sds_new_with_len(const char *init, int len)
{
    sds_hdr_t *sh = (sds_hdr_t *)malloc(sizeof(sds_hdr_t) + len + 1);
    if (sh == NULL) {
        return NULL;
    }

    if (init != NULL) {
        memcpy(sh->buf, init, len);
        sh->free = 0;
        sh->len = len;
    } else {
        memset(sh->buf, 0, len);
        sh->free = len;
        sh->len = 0;
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

static sds_hdr_t *sds_get_hdr(sds_t obj)
{
    return (sds_hdr_t *)(obj - sizeof(sds_hdr_t));
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
        diag_err("Create substr failed, src %s, start %d, end %d.",
            (char *)src, start, end);
        return NULL;
    }
    (void)memcpy(substr, src + start, end - start);
    return substr;
}

static uint32_t sds_calc_new_space_size(uint32_t needed_space)
{
    /* 内存增长策略：
        1：需要的空间space小于1M时，申请 2 * space的空间。
        2：需要的空间space大于1M是，申请 space + 1M的空间
    */
    if (needed_space < (1 * 1024 * 1024)) {
        return 2 * needed_space;
    } else {
        return needed_space + (1 * 1024 * 1024);
    }
}

static uint32_t sds_make_space(sds_t s, uint32_t needed_space)
{
    sds_hdr_t *hdr = sds_get_hdr(s);
    if (hdr->len + hdr->free > needed_space) {
        return s;
    }
    uint32_t new_len = sds_calc_new_space_size(needed_space);
    sds_hdr_t *new_hdr = (sds_hdr_t *)calloc(1, sizeof(*new_hdr) + new_len + 1);
    if (new_hdr == NULL) {
        diag_err("[sds] Malloc failed");
        sds_free(s);
        return NULL;
    }

    new_hdr->len = hdr->len;
    new_hdr->free = new_len - hdr->len;

    sds_t new_obj = (sds_t)(new_hdr + 1);
    (void)strcpy(new_obj, s);
    sds_free(s);
    return new_obj;
}

static sds_t sds_cat_with_len(sds_t s, void *t, uint32_t len)
{
    if (s == NULL) {
        s = sds_new_with_len(t, len);
        if (s == NULL) {
            diag_err("[sds] Create new sds failed.");
            return NULL;
        }
    }

    hdr_t *hdr = sds_get_hdr(s);

    s = sds_make_space(s, hdr->len + hdr->free + len);
    if (s == NULL) {
        diag_err("[sds] Malloc failed");
        return NULL;
    }

    /* 动态数组已保证目标地址空间是够的 */
    (void)strcpy(s + hdr->len, (char *)t);
    return s;
}

sds_t sds_cat(sds_t obj, const char *t)
{
    return sds_cat_with_len(obj, t, strlen(t));
}
