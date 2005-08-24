#include "server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <linux/netfilter_ipv4/ip_nat.h>
#define BAD_MAX_VALUE -1000

static struct conn_t *get_address(int connfd);

int create_server_socket(struct in_addr host, uint16_t port)
{
    int error = 0;
    int fd;
    int option_value = 1;
    struct sockaddr_in new_server;
	
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	syslog(LOG_INFO, "could not create server socket");
    memset(&new_server,0,sizeof(struct sockaddr_in));
    new_server.sin_family = AF_INET;
    new_server.sin_addr.s_addr = host.s_addr;
    new_server.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &new_server, sizeof(new_server)) == -1)
	syslog(LOG_ERR, "could not bind to %s ", strerror(errno));
    if (listen(fd, MAX_BACKLOG) == -1)
	syslog(LOG_ERR, "could not listen on server socket");
      
    error = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		       (char *) &option_value, sizeof(option_value));
    if (error == -1)
	syslog(LOG_ERR, "could not set socket options");
    return fd;
}

static int maxfd (int *array, size_t size)
{
    int k;
    int max = BAD_MAX_VALUE;
    for (k=0;k<size;k++)
    {
	if (array[k]>max)
	{
	    max = array[k];
	}
    }
    return max;
}
int multiaccept (int *listen_array, size_t size, struct conn_t *state)
{

    fd_set readfds;
    int c;
    int j;
    struct timeval timer;
    struct sockaddr_in socket;
    struct conn_t *tmp;
    socklen_t len;

    timer.tv_sec = timer.tv_usec = 0;

    int maxfd_value = maxfd(listen_array,size);

    if (maxfd_value == BAD_MAX_VALUE)
    {
	return -1;
    }

    FD_ZERO(&readfds);

    for (j=0;j<size;j++)
    {
	FD_SET(listen_array[j], &readfds);
    }
    if (select(maxfd_value+1, &readfds, NULL, NULL, &timer))
    {
	for (j=0;j<size;j++)
	{
	    if (FD_ISSET(listen_array[j],&readfds))
	    {
                len = sizeof(socket);
		c = accept(listen_array[j],(struct sockaddr *) &socket,&len);
		if (c >= 0)
		{
		    tmp = get_address(c);
		    if (tmp == NULL)
		    {
			fprintf(stderr,"%s: address allocation error",__func__);
			exit(EXIT_FAILURE);
		    }
		   
		    memcpy(state,tmp,sizeof(struct conn_t));
		    free(tmp);
		    state->src_ip = ntohs(socket.sin_addr.s_addr);
		    state->src_port = ntohs(socket.sin_port);
		    state->connfd = c;
		}
		memset(&socket,0,sizeof(struct sockaddr_in));
		len = 0;
	    }
	}
    }
    return 0;
}
static struct conn_t *get_address(int connfd)
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    struct conn_t *address;
	
	
    if (getsockopt(connfd, SOL_IP, SO_ORIGINAL_DST, &saddr, &len) != 0) {
	fprintf(stderr,"%s: cannot get natted address",__func__);
	exit(EXIT_FAILURE);
    }
    address = calloc(1,sizeof(struct conn_t));
    if (address)
    {
	address->dst_ip = ntohs(saddr.sin_addr.s_addr);
	address->dst_port = ntohs(saddr.sin_port);
    }
    return address;
}


 
