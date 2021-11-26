#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"
#define maxN 1024

typedef struct
{
    int st;
    char *str;
} response;

void *recvsocket(void *args)
{
    int st = *(int *)args;
    char str[maxN];
    while (recv(st, str, sizeof(str), 0) > 0)
    {
        if (strlen(str))
        {
            printf("recv: %s\n", str);
            send(st, str, strlen(str), 0);
            printf("send: %s\n", str);
        }
        memset(str, 0, sizeof(str));
    }
    return NULL;
}

int main(int argc, char **argv)
{
    char *server_port = 0;
    int opt = 0;
    /* Parsing args */
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "Unknown option \"-%c\"\n", isprint(optopt) ? optopt : '#');
            return 0;
        }
    }

    if (!server_port)
    {
        fprintf(stderr, "Error! No port number provided!\n");
        exit(1);
    }

    // init socket
    int port = atoi(server_port);
    int st = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
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

    int client_st = 0; // socket client
    struct sockaddr_in client_addr;

    pthread_t thrd;
    while (1)
    {
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);

        // accept will block until client connect
        client_st = accept(st, (struct sockaddr *)&client_addr, &len);
        if (client_st == -1)
        {
            printf("accept failed %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        printf("accepted %s\n", inet_ntoa(client_addr.sin_addr));

        pthread_create(&thrd, NULL, recvsocket, &client_st);
        pthread_detach(thrd);
    }

    close(st);
    return EXIT_SUCCESS;
}
