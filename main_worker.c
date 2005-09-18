
#include "ipcfsm.h"
#include "socket/msocket.h"
#include "queue/queue.h"
#include "thread.h"
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void main_worker(int *sock)
{

    struct conn_t state;
    work_item w;
    work_item z;
    IO_QUEUE input;
    memset(&input,0,sizeof(input));
    create_queue(&input, CONN_QUEUE);
    create_in_farm(&input, 2);
    memset(&w,0,sizeof(w));

    for (;;)
    {
	
        		
	server_fsm(sock,&state);
	fprintf(stderr,"%s: Obtained file descriptor: %d\n",__func__,state.connfd);
	memcpy(&w.state,&state,sizeof(w.state));
	memset(&state,0,sizeof(state));
	fprintf(stderr,"%s: Sending file descriptor to queue\n",__func__);
	
	//send_queue(w,&inq);
	
	write_item(w,&inq);
	void *ptr = inq.tail;
	ptr+=14;
	unsigned int *value = (unsigned int*) ptr;
	fprintf(stderr,"RETURN PTR:%p, VALUE:%d\n",value,*value);
	z = read_item(&inq);
	

    }
    destroy_queue(&inq);
}

