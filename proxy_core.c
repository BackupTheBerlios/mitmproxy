/*
 Copyright (c) 2005 Giorgio Zoppi <zoppi@cli.di.unipi.it>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/* System libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include "conf/conf.h"
#include "conf/proxyc.h"
#include <fcntl.h>
#include "utils.h"
#include "pidfile.h"
#include "server.h"
#include "strlcpy.h"
#include "fdpass.h"
#include "main_worker.h"
#define DEBUG_CORE


int proxy_core(struct proxybind *header, int num_items, const char *chdir, char *user)
{	    
    

    /* needed for IPC between parent and child in this two model process */

    int thesockets[2];
    pid_t pid;
    int status;
    struct conn_t state;
    struct proxybind *tmpbind;
   
    int *listen_array = calloc(num_items,sizeof(int));
/*
    int *conn_array = calloc(num_items,sizeof(int));
    struct sockaddr_in *cliaddr_array = calloc(num_items,sizeof(struct sockaddr_in)); 
*/

    if (listen_array == NULL)
	{
	fprintf(stderr,"%s: Memory allocation error\n",__func__);
	exit(EXIT_FAILURE);
	}

    if (socketpair(AF_UNIX,SOCK_STREAM,0,thesockets)<0)
    {
	fprintf(stderr,"%s: Error during setting socketpair IPC\n",__func__);
	exit(EXIT_FAILURE);
    }
    tmpbind = header;

    int j;

    for (j = 0; j < num_items; j++)
    {
    listen_array[j] = create_server_socket(tmpbind->ipaddress,tmpbind->port); 
    #ifdef DEBUG_CORE
    fprintf(stderr,"Opening socket on port %d\n",tmpbind->port);
    #endif
    tmpbind = tmpbind->next;
    }
   
    if ((pid = fork())<0)
    {
	fprintf(stderr,"%s: Fork failed before IPC\n",__func__);
	exit(EXIT_FAILURE);
    } 
    if (pid)
    {
    /* parent
    1) open file descriptor
    2) send file descriptor to child.
    */

    close(thesockets[0]);

    for (;;)
    {
	if (multiaccept (listen_array, num_items, &state)< 0)
	{
	    fprintf(stderr,"Error");
	    exit(EXIT_FAILURE);
	    
	}
    #ifdef DEBUG_CORE
    fprintf(stderr,"Sending fd to worker  %d on sockets %d \n",state.connfd,thesockets[1]);
    #endif
    send_fd(thesockets[1],state.connfd);
    memset(&state,0,sizeof(struct conn_t));
    }       while (waitpid(-1,&status,WNOHANG)<0)
	{
	fprintf(stderr,"%s: waitpid error\n",__func__);
	exit(EXIT_FAILURE);
	}
    if (WIFEXITED(status))
	{
	fprintf(stderr,"%s: program terminated\n",__func__); 
	free(listen_array);	
	exit(EXIT_FAILURE);
	}
    }
    else {
    /*child
	if (jchroot(chdir))
	{
	    fprintf(stderr,"%s: cannot chroot to %s\n",__func__,chdir);
	    exit(EXIT_FAILURE);
	}
*/
    
    if (!drop_privs(user))
    {

	fprintf(stderr,"%s: cannot drop privilegies to %s\n",__func__,user);
	exit(EXIT_FAILURE);
    }
 #ifdef DEBUG_CORE
    fprintf(stderr,"Starting worker\n");
   
    #endif

    close(thesockets[1]);
    
   
    // main_worker(thesockets);
}

return 0; 
}

