#ifndef SERVER_H_
#define SERVER_H

#include "protocl.h"

int listen_start(int port);
int accpet_client(int sockfd);
Pack_t*  recv_msg(int sockfd);
void send_msg(int sockfd,void* buf,int len);

#endif