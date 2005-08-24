#ifndef IPC_FSM
#define IPC_FSM
#include "server.h"

typedef unsigned int p_msg;
#define CLIENT_SOCK sock[1]
#define SERVER_SOCK sock[0]
#define SERVER_GREET 100
#define CLIENT_GREET 101
#define IPBUFFER  6
#define GREET_SIZE sizeof(unsigned int)
#define RESULT_SIZE sizeof(unsigned int)
#define OK 102
#define NOT_OK 103
int client_fsm(int *sock,struct conn_t state);
int server_fsm(int *sock,struct conn_t *state);
#endif 

