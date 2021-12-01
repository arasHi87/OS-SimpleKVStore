#include "format.h"
#include "log.h"
#include "sock.h"
#include "treap.h"
#include "types.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Node *map = NULL, *tmp;

char *deal_req(char *input)
{
    char cmd[MAX_LEN], key[MAX_LEN], val[MAX_LEN];
    char *resp = (char *)malloc(sizeof(char) * MAX_LEN);

    if (!strncmp(input, "SET", 2))
    {
        if (sscanf(input, "%s %s %s", cmd, key, val) != 2)
            if (find(map, key))
                sprintf(resp, "key %s already exist", key);
            else
            {
                map = insert(map, key, val);
                sprintf(resp, "key value pair (%s, %s) is stored!", key, val);
            }
        else
            sprintf(resp, "command format error, please check again");
    }
    else if (!strncmp(input, "GET", 2))
    {
        if (sscanf(input, "%s %s", cmd, key) != 1)
        {
            tmp = find(map, key);
            if (tmp)
                sprintf(resp, "the value of %s is %s", tmp->key, tmp->val);
            else
                sprintf(resp, "the key is not exist!");
        }
        else
            sprintf(resp, "command format error, please check again");
    }
    else if (!strncmp(input, "DEL", 2))
    {
        if (sscanf(input, "%s %s", cmd, key) != 1)
        {
            tmp = find(map, key);
            if (!tmp)
                sprintf(resp, "the key is not exist!");
            else
            {
                map = destroy(map, key);
                sprintf(resp, "key %s is removed", key);
            }
        }
        else
            sprintf(resp, "command format error, please check again");
    }
    else
        sprintf(resp, "command not found, please cheack again");
    return resp;
}

void *recvsocket(void *args)
{
    int st = *(int *)args;
    char str[MAX_LEN];
    while (recv(st, str, sizeof(str), 0) > 0)
    {
        if (strlen(str))
        {
            trim(str);
            log_info("recv: %s", str);
            char *result = deal_req(str);
            send(st, result, strlen(result), 0);
            log_info("send: %s", result);
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
            log_error("Unknown option \"-%c\"\n", isprint(optopt) ? optopt : '#');
            return 0;
        }
    }

    if (!server_port)
    {
        log_error("Error! No port number provided!");
        return EXIT_FAILURE;
    }

    int skt = listenfd(atoi(server_port)); // init server socket
    int client_skt = 0;                    // socket client
    struct sockaddr_in client_addr;

    pthread_t thrd;
    while (1)
    {
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);

        // accept will block until client connect
        client_skt = accept(skt, (struct sockaddr *)&client_addr, &len);
        if (client_skt == -1)
        {
            log_error("accept failed %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        log_info("accepeted %s:%d", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

        pthread_create(&thrd, NULL, recvsocket, &client_skt);
        pthread_detach(thrd);
    }

    close(skt);
    return EXIT_SUCCESS;
}
