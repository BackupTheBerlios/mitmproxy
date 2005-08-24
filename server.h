#ifndef _SERVER_H
#define _SERVER_H
#define MAX_BACKLOG     5
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int create_server_socket(struct in_addr host, uint16_t port); 
int multiaccept (int *listen_array, int *fd_conn_array, struct sockaddr_in *cliaddr_array, size_t size);
#endif
