
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
#include "http.h"

#define URL "http://www.jiedev.com"

int main(int argc, char **argv)
{
    http_get(URL);
    return 0;
}