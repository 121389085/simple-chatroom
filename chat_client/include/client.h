#ifndef CLIENT_H_
#define CLIENT_H_
#include "protocl.h"

int connect_server(char *ip,int port);
Pack_t* recv_msg(int sockfd);
void send_msg(int sockfd,void* buf,int len);

#endif