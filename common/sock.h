#ifndef _SOCK_H_
#define _SOCK_H_

int listenfd(int port);
int clientfd(int host, int port);

#endif