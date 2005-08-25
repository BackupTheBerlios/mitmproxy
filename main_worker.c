#include "ipcfsm.h"
#include "server.h"
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static void *doit(void *);

void main_worker(int *sock)
{
    pthread_t tid;
    struct conn_t state;
    int *iptr;	
    for (;;)
    {
	
	iptr = calloc(1,sizeof(int));
//	*iptr = receive_fd(sock[0]);
	
	server_fsm(sock,&state);
	*iptr = state.connfd; 
	fprintf(stderr,"%s: Received fd -> %d\n",__func__,*iptr);
	pthread_create(&tid,NULL,&doit,iptr);
       
	
    }
}

static void *doit(void *arg)
{
    const char msg[] = "TCP\n";
    int connfd = *((int *) arg); 
    free(arg);
    pthread_detach(pthread_self());
    fprintf(stderr,"%s: Writing on fd -> %d\n",__func__,connfd);
    write(connfd,(const char*) msg, strlen(msg));
    close(connfd);
    return(NULL);
}
