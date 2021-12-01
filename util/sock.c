#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int listenfd(int port)
{
    // init socket
    int st = socket(AF_INET, SOCK_STREAM, 0), on = 1;
    if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        printf("init socket failed, %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // define an ip address struct
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;                // define addr to TCP/IP
    addr.sin_port = htons(port);              // convert local byte order to network byte order
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // all adress on this host

    // bind ip to the server
    if (bind(st, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("bind socket failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // server start listen
    if (listen(st, 20) == -1)
    {
        printf("listen failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return st;
}

int clientfd(int host, int port)
{
    int st = socket(AF_INET, SOCK_STREAM, 0);

    // define an ip address struct
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;          // define addr to TCP/IP
    addr.sin_port = htons(port);        // convert local byte order to network byte order
    addr.sin_addr.s_addr = htonl(host); // all adress on this host

    // use connect to connect the ip address and port which addr define
    if (connect(st, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("connect failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return st;
}