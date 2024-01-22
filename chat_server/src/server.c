#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"


//启动监听
int listen_start(int port)
{
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd,(struct sockaddr*)&addr,sizeof(addr))<0)
    {
        perror("ip地址绑定失败");
        close(listenfd);
        exit(1);
    }
    if(listen(listenfd,20)<0)
    {
        perror("监听启动失败");
        close(listenfd);
        exit(1);
    }
    return listenfd;
}


//接收并同意连接客户端
int accpet_client(int sockfd)
{
    struct sockaddr_in addr = {};
    socklen_t len = sizeof(addr);
    int clientfd = accept(sockfd,(struct sockaddr*)&addr,&len);
    if(clientfd <0)
    {
        perror("客户端连接失败");
        return -1;
    }
    else
    {
        printf("新客户端[%s]在%d端口建立连接\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
        return clientfd;
    }
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
