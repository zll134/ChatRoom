/* ********************************
 * Author:       Zhanglele
 * Description:  url模块
 * create time: 2022.12.11
 ********************************/
#include "url.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "sds.h"

#define SCHEME_MAX_LEN 64

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define SCHEME_DELIM "://"
#define AUTH_DELIM "@"
#define HOST_DELIM "/"
#define PATH_DELIM ";"
#define PARAMS_DELIM "?"
#define QUERY_DELIM "#"

#define HTTP_PORT 80
#define HTTPS_PORT 443

/* URI Schemes */
static const char *URL_SCHEMES[] = {
    "http", "HTTP", "https", "HTTPS"
};

static bool url_is_scheme(const char *scheme)
{
    for (int i = 0; i < ARRAY_SIZE(URL_SCHEMES); i++) {
        if (strcmp(URL_SCHEMES[i], scheme) == 0) {
            return true;
        }
    }
    return false;
}

static int url_get_scheme(sds_t urlStr, int *start_pos, url_data_t *params)
{
    const char *str = sds_get_string(urlStr);
    char scheme[SCHEME_MAX_LEN] = {0};
    (void)sscanf(urlStr, "%[^://]", scheme);

    if (!url_is_scheme(scheme)) {
        return -1;
    }
    params->scheme = sds_new(scheme);
    if (params->scheme == NULL) {
        return -1;
    }
    *start_pos = *start_pos + sds_get_len(params->scheme) + strlen(SCHEME_DELIM);
    return 0;
}

/* 获取url中的用户名和密码。
   url中包含用户名和密码的例子:
    1、ftp://ftp.prep.ai.mit.edu/pub/gnu
    2、ftp://anonymous@ftp.prep.ai.mit.edu/pub/gnu
    3、ftp://anonymous:password@ftp.prep.ai.mit.edu/pub/gnu
 */
static int url_get_auth(sds_t urlStr, int *start_pos, url_data_t *params)
{
    uint32_t url_len = sds_get_len(urlStr);
    int end_pos = sds_find_str(urlStr, *start_pos, url_len, AUTH_DELIM);
    if (end_pos == INVALID_POS) {
        return 0;
    }

    int delim_pos = sds_find_str(urlStr, *start_pos, end_pos, ":");
    if (delim_pos == INVALID_POS) {
        params->user = sds_substr(urlStr, *start_pos, end_pos);
        if (params->user == NULL) {
            return -1;
        }
    } else {
        params->user = sds_substr(urlStr, *start_pos, delim_pos);
        if (params->user == NULL) {
            return -1;
        }
        params->password = sds_substr(urlStr, delim_pos + 1, end_pos);
        if (params->password == NULL) {
            return -1;
        }
    }
    *start_pos = *start_pos + sds_get_len(params->user) +
        sds_get_len(params->password) + strlen(AUTH_DELIM);
    return 0;
}

static int url_get_default_port(sds_t schema)
{
    if (strcmp((char *)schema, "http") == 0) {
        return HTTP_PORT;
    } else {
        return HTTPS_PORT;
    }
}

static int url_get_host_addr(sds_t urlStr, int *start_pos, url_data_t *params)
{
    uint32_t url_len = sds_get_len(urlStr);
    int end_pos = sds_find_str(urlStr, *start_pos, url_len, HOST_DELIM);
    if (end_pos == INVALID_POS) {
        end_pos = url_len;
    }

    int delim_pos = sds_find_str(urlStr, *start_pos, end_pos, ":");
    if (delim_pos == INVALID_POS) {
        params->host = sds_substr(urlStr, *start_pos, end_pos);
        if (params->host == NULL) {
            diag_err("[url]Get substr of host failed when delim not exist, "
                     "url: %s, startpos %d, end pos %d.", (char *)urlStr, *start_pos, end_pos);
            return -1;
        }
        params->port = url_get_default_port(params->scheme);
        *start_pos = *start_pos + end_pos;
    } else {
        params->host = sds_substr(urlStr, *start_pos, delim_pos);
        if (params->host == NULL) {
            diag_err("[url]Get substr of host failed when delim exist, "
                     "url: %s, startpos %d, end pos %d.", (char *)urlStr, *start_pos, end_pos);
            return -1;
        }

        sds_t port_str = sds_substr(urlStr, delim_pos, end_pos);
        if (port_str == NULL) {
            diag_err("[url]Get substr of port failed when delim not exist, "
                     "url: %s, startpos %d, end pos %d.", (char *)urlStr, *start_pos, end_pos);
            return -1;
        }
        params->port = atoi((char *)port_str);
        sds_free(port_str);
        *start_pos = *start_pos + sds_get_len(params->host) + sds_get_len(port_str);
    }
    diag_info("[url]Get host and port from url success, host %s, port %d.",
        (char *)params->host, params->port);
    return 0;
}

static int url_get_path(sds_t urlStr, int *start_pos, url_data_t *params)
{
    uint32_t url_len = sds_get_len(urlStr);
    if (*start_pos >= url_len) {
        return 0;
    }

    int end_pos = sds_find_str(urlStr, *start_pos, url_len, PATH_DELIM);
    if (end_pos == INVALID_POS) {
        end_pos = url_len;
    }

    params->path = sds_substr(urlStr, *start_pos, end_pos);
    if (params->user == NULL) {
        return -1;
    }

    *start_pos = *start_pos + sds_get_len(params->path) + strlen(PATH_DELIM);
    return 0;
}

static int url_get_params(sds_t urlStr, int *start_pos, url_data_t *params)
{
    uint32_t url_len = sds_get_len(urlStr);
    if (*start_pos >= url_len) {
        return 0;
    }

    int end_pos = sds_find_str(urlStr, *start_pos, url_len, PARAMS_DELIM);
    if (end_pos == INVALID_POS) {
        end_pos = url_len;
    }

    params->params = sds_substr(urlStr, *start_pos, end_pos);
    if (params->params == NULL) {
        return -1;
    }

    *start_pos = *start_pos + sds_get_len(params->params) + strlen(PARAMS_DELIM);
    return 0;
}

static int url_get_query(sds_t urlStr, int *start_pos, url_data_t *params)
{
    uint32_t url_len = sds_get_len(urlStr);
    if (*start_pos >= url_len) {
        return 0;
    }

    int end_pos = sds_find_str(urlStr, *start_pos, url_len, QUERY_DELIM);
    if (end_pos == INVALID_POS) {
        end_pos = url_len;
    }

    params->query = sds_substr(urlStr, *start_pos, end_pos);
    if (params->query == NULL) {
        return -1;
    }

    *start_pos = *start_pos + sds_get_len(params->query) + strlen(QUERY_DELIM);
    return 0;
}

static int url_get_frag(sds_t urlStr, int *start_pos, url_data_t *params)
{
    uint32_t url_len = sds_get_len(urlStr);
    if (*start_pos >= url_len) {
        return 0;
    }

    params->frag = sds_substr(urlStr, *start_pos, url_len);
    if (params->query == NULL) {
        return -1;
    }

    *start_pos = *start_pos + sds_get_len(params->frag);
    return 0;
}

void url_free(url_data_t *params)
{
    if (params->url != NULL) {
        sds_free(params->scheme);
    }
    if (params->scheme != NULL) {
        sds_free(params->scheme);
    }
    if (params->user != NULL) {
        sds_free(params->user);
    }
    if (params->password != NULL) {
        sds_free(params->password);
    }
    if (params->host != NULL) {
        sds_free(params->host);
    }
    if (params->path != NULL) {
        sds_free(params->path);
    }
    if (params->params != NULL) {
        sds_free(params->params);
    }
    if (params->query != NULL) {
        sds_free(params->query);
    }
    if (params->frag != NULL) {
        sds_free(params->frag);
    }
}

/* url:<scheme>://<user>:<password>@<host>:<port>/<path>;<params>?<query>#<frag>
 * test:"https://jiejie:test@jiedev.com:8080/test/index.php?who=jiejie#frag"
 */
int url_parse(const char *urlStr, url_data_t *params)
{
    if (urlStr == NULL) {
        return -1;
    }

    params->url = sds_new(urlStr);
    if (params->url  == NULL) {
        return -1;
    }

    int start_pos = 0;
    int ret = url_get_scheme(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get scheme from url failed, url %s.", (char *)urlStr);
        return -1;
    }

    ret = url_get_auth(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get auth from url failed, url %s.", (char *)urlStr);
        url_free(params);
        return -1;
    }

    ret = url_get_host_addr(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get host from url failed, url %s.", (char *)urlStr);
        url_free(params);
        return -1;
    }

    ret = url_get_path(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get path from url failed, url %s.", (char *)urlStr);
        url_free(params);
        return -1;
    }

    ret = url_get_params(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get params from url failed, url %s.", (char *)urlStr);
        url_free(params);
        return -1;
    }

    ret = url_get_query(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get query from url failed, url %s.", (char *)urlStr);
        url_free(params);
        sds_free(urlStr);
        return -1;
    }

    ret = url_get_frag(params->url, &start_pos, params);
    if (ret != 0) {
        diag_err("[url]Get frag from url failed, url %s.", (char *)urlStr);
        url_free(params);
        return -1;
    }
    return 0;
}

void url_print_info(url_data_t *params)
{
    printf("url info: \n");
    if (params->scheme != NULL) {
        printf("  scheme: %s\n", (char *)params->scheme);
    }
    if (params->user != NULL) {
        printf("  usr: %s\n", (char *)params->user);
    }
    if (params->password != NULL) {
        printf("  password: %s\n", (char *)params->password);
    }
    if (params->host != NULL) {
        printf("  host: %s\n", (char *)params->host);
    }
    printf("  port: %d\n", params->port);
    if (params->path != NULL) {
        printf("  path: %s\n", (char *)params->path);
    }
    if (params->params != NULL) {
        printf("  params: %s\n", (char *)params->params);
    }
    if (params->query != NULL) {
        printf("  query: %s\n", (char *)params->query);
    }
    if (params->frag != NULL) {
        printf("  frag: %s\n", (char *)params->frag);
    }
}