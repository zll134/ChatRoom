/* ********************************
 * Author:       Zhanglele
 * Description:  hash表管理模块
 * create time:  2023.05.21
 ********************************/

#include "dict.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"

dict_t *dict_create(dict_config_t *config)
{
    dict_t *dict = calloc(1, sizeof(dict_t));
    if (!dict) {
        diag_err("[dict] Create dict failed");
        return NULL;
    }

    (void)memcpy(&dict->config, config, sizeof(*config));
    return dict;
}

void dict_destroy(dict_t *dict)
{
    if (dict != NULL) {
        free(dict);
    }
}