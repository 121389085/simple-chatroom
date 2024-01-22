#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

//连接服务器
int connect_server(char *ip,int port)
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
    {
        perror("连接服务器失败");
        close(sockfd);
        exit(1);
    }
    else
    {
        printf("成功连接服务器\n");
    }
    return sockfd;
}

//接收消息
Pack_t* recv_msg(int sockfd)
{
    int len = 0;
    recv(sockfd,&len,sizeof(len),0);
    Pack_t* pack = calloc(1,len); //根据长度声明动态空间
    pack->pack_len = len;
    int bytes = recv(sockfd,pack,len,0);
    if(bytes <=0)
    {
        return NULL;
    }
    return pack;
}

//发送消息
void send_msg(int sockfd,void* buf,int len)
{
    send(sockfd,&len,sizeof(int),0);
    send(sockfd,buf,len,0);
}
