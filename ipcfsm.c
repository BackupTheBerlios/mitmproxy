#include "fdpass.h"
#include "ipcfsm.h"
#include "server.h"
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int client_fsm(int *sock, struct conn_t state)
{
    p_msg out;
    p_msg in;

    char buffer[IPBUFFER];
    

    memset(buffer,0,sizeof(buffer)-1);
    fprintf(stderr,"%s: Greet the server",__func__);
    /* Greet the server */
    in = CLIENT_GREET;
    write(CLIENT_SOCK,&in,sizeof(p_msg));
    fprintf(stderr,"%s: Receive greet from the server\n",__func__);
    /* Receive greet from the server */
    read(CLIENT_SOCK,&out,sizeof(p_msg));
    
    /* Send ip quadruple to the server 
      integer:interger:integer:integer
    */
    fprintf(stderr,"%s: Send ip quadruple\n",__func__);
    snprintf(buffer,sizeof(buffer),"%u:%u:%u:%u",state.dst_ip,state.dst_port,state.src_ip,state.src_port);
   
    write(CLIENT_SOCK,buffer,IPBUFFER);
    fprintf(stderr,"%s: Receive result from the server\n",__func__);
    /* Receive resutl from the server */
    read(CLIENT_SOCK,&out,RESULT_SIZE);
    /* Send file descriptor*/
    fprintf(stderr,"%s: Send file descriptor\n",__func__);
    send_fd(CLIENT_SOCK,state.connfd);
    /* Receive result from the server*/
    fprintf(stderr,"%s: Receive result from the server",__func__);
    read(CLIENT_SOCK,&out,RESULT_SIZE);
    return 0;
    
}
int server_fsm(int *sock,struct conn_t *s)
{
    p_msg out;
    p_msg in;
    char buff[IPBUFFER];
    char tmp[IPBUFFER];
    
    unsigned int fd[4];
    int j,k,i;

    memset(buff,0,IPBUFFER -1);
    memset(tmp,0, IPBUFFER -1);
    /* Wait for greeting */
    read(SERVER_SOCK,&in,sizeof(p_msg));
    /* Greet the client */
    out = SERVER_GREET;
    write(SERVER_SOCK,&out,sizeof(p_msg));
    /* Wait for ip quadruple*/
    read(SERVER_SOCK,buff,sizeof(buff));
    
    /* Send result  */
    out = OK;
    write(SERVER_SOCK,&out,sizeof(p_msg));
    j = 0;
    i = 0;
    /* Unpack addresses */
    for (k = 0; k < strlen(buff); k++)
    {
	if (buff[k] != ':')
	{
	tmp[j] = buff[k];
	j++;
	}
	else {
	    fd[i] = atoi(tmp);
	    j = 0;
            i++;
	    memset(tmp,0,sizeof(tmp)-1);
	}

    }
    s->dst_ip = fd[0];
    s->dst_port = fd[1];
    s->src_ip = fd[2];
    s->src_port = fd[3];
    

    /* Wait for file descrip*/
    s->connfd = receive_fd(SERVER_SOCK);
    out = OK;
    write(SERVER_SOCK,&out,sizeof(p_msg));
    
    return 0;
    
}
