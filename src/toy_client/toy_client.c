
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"
#include "sds.h"
#include "url.h"
#include "net.h"

#define URL "http://www.jiedev.com"

int requests(url_data_t *conn_params)
{
    /* Build the address. */
    struct hostent *hostent = gethostbyname((char *)conn_params->host);
    if (hostent == NULL) {
        diag_err("Gethostbyname failed, host %s, error %d.", (char *)conn_params->host, h_errno);
        return -1;
    }

    for(int i= 0; hostent->h_addr_list[i]; i++){
        diag_info("IP addr %d: %s", i+1, inet_ntoa(*(struct in_addr*)hostent->h_addr_list[i]));
    }

    int fd = net_connect(inet_ntoa(*(struct in_addr*)hostent->h_addr_list[0]), conn_params->port);
    if (fd < 0) {
        diag_err("Connect to server failed");
        return -1;
    }
    char request_buf[] = {
        "GET / HTTP/1.1\r\n"
        "Host: www.jiedev.com\r\n"
        "\r\n"
    };
    int n = net_write(fd, request_buf, strlen(request_buf));

    char response[4096] = {0};
    net_read(fd, response, sizeof(response));
    printf("%s\n", response);
    return 0;
}

int main(int argc, char **argv)
{
    url_data_t conn_params = {0};
    int ret = url_parse(URL, &conn_params);
    if (ret != 0) {
        diag_err("Parse url failed, url: %s.", (char *)urlStr);
        return -1;
    }
    url_print_info(&conn_params);
    requests(&conn_params);
    url_free(&conn_params);
    return 0;
}