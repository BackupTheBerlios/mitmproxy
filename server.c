#include "server.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
    int max = -1000;
    for (k=0;k<size;k++)
    {
	if (array[k]>max)
	{
	    max = array[k];
	}
    }
    return max;
}
int multiaccept (int *listen_array, int *fd_conn_array, struct sockaddr_in *cliaddr_array, size_t size)
{

fd_set readfds;
int connfd;
int j;
int maxfd_value = maxfd(listen_array,size);
struct timeval timer;
timer.tv_sec = timer.tv_usec = 0;

FD_ZERO(&readfds);

 for (j=0;j<size;j++)
 {
 FD_SET(listen_array[j], &readfds);
 }
if (select(maxfd_value+1, &readfds, NULL, NULL, &timer))
{
    for (j=0;j<size;j++)
    {
	if (FD_ISSET(listen_array[j],&readfs))
	{
	connfd = accept(listen_array[j],(struct sockaddr *),&, &m_SocketAddrLength);
	}
    }
}

}
 
