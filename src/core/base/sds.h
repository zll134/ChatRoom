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

#define INVALID_POS (-1)

sds_t sds_new_with_len(const char *init, int len);
sds_t sds_new(const char *init);
void sds_free(sds_t obj);
const char *sds_get_string(sds_t obj);
int sds_find_str(sds_t obj, uint32_t start, uint32_t end, const char *str);
uint32_t sds_get_len(sds_t obj);
sds_t sds_substr(sds_t src, int start, int end);

/* 将字符串t追加到动态数组obj后面 */
sds_t sds_cat(sds_t obj, const char *t);

/*  可变字符串添加到动态数组后面。
    参数: obj 动态数组
        format: 格式化字符串。用于控制输入的可变参数，当前只支持%s。
 */
sds_t sds_vcat(sds_t obj, const char *format, ...);

#endif