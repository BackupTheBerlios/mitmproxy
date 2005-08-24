#include "fdpass.h"
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
    int *iptr;	
    for (;;)
    {
	
	iptr = calloc(1,sizeof(int));
	*iptr = receive_fd(sock[0]);
	fprintf(stderr,"%s: Received fd -> %d\n",__func__,*iptr);

	pthread_create(&tid,NULL,&doit,iptr);
       
	
    }
}

static void *doit(void *arg)
{
    const char msg[] = "TCP proxy Server 0.1\n";
    int connfd = *((int *) arg); 
    int z = 0;
    free(arg);
    pthread_detach(pthread_self());
    fprintf(stderr,"%s: Writing on fd -> %d\n",__func__,connfd);
    for (z = 0; z < 20; z++)
    {
    write(connfd,(const char*) msg, strlen(msg));
    }
    close(connfd);
    return(NULL);
}
