#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define maxN 1024

void *recvsocket(void *args)
{
    int st = *(int *)args;
    char str[maxN];
    while (recv(st, str, sizeof(str), 0) > 0)
    {
        printf("recv: %s\n", str);
        memset(str, 0, sizeof(str));
    }
    return NULL;
}

void *sendsocket(void *args)
{
    int st = *(int *)args;
    char str[maxN];
    while (~read(STDIN_FILENO, str, sizeof(str)))
    {
        printf("send: %s\n", str);
        send(st, str, sizeof(str), 0);
        memset(str, 0, sizeof(str));
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int opt;
    char *server_host_name = NULL, *server_port = NULL;

    /* Parsing args */
    while ((opt = getopt(argc, argv, "h:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            server_host_name = malloc(strlen(optarg) + 1);
            strncpy(server_host_name, optarg, strlen(optarg));
            break;
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "Unknown option \"-%c\"\n", isprint(optopt) ? optopt : '#');
            return 0;
        }
    }

    if (!server_host_name)
    {
        fprintf(stderr, "Error!, No host name provided!\n");
        exit(1);
    }

    if (!server_port)
    {
        fprintf(stderr, "Error!, No port number provided!\n");
        exit(1);
    }

    // init socket
    int port = atoi(server_port);
    int host = atoi(server_host_name);
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

    // recv response and send data to server
    pthread_t thrd1, thrd2;
    pthread_create(&thrd1, NULL, recvsocket, &st);
    pthread_create(&thrd2, NULL, sendsocket, &st);
    pthread_join(thrd1, NULL);
    close(st);

    return 0;
}
