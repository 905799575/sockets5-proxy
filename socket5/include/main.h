#ifndef __MAINH__
#define __MAINH__
#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <math.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BACK_LOG 1024
#define EPOLL_SIZE 1024
#define MAXBUFF 1024

int Socket(int domain, int type, int protocol);
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
int Epoll_create(int size);
int Setnoblocking(int fd);
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
int Setreuseaddr(int fd);
int handle_client_connect(int client_sock);
void sigchild_handle(int signum);
int forward_data(int cli_fd, int remote_fd);
#endif