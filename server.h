#ifndef _SERVER_H
#define _SERVER_H
#define MAX_BACKLOG     5
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct conn_t {
    unsigned int dst_ip;
    unsigned int dst_port;
    unsigned int src_ip;
    unsigned int src_port;
    unsigned int connfd;
};
int create_server_socket(struct in_addr host, uint16_t port); 
int multiaccept (int *listen_array, size_t size, struct conn_t *state);
#endif
