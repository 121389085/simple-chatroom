#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "client.h"
#include "main_face.h"
#include "protocl.h"
#include "user_face.h"

void login(int sockfd)
{
    //输入基本信息
    char pswd[21]={},name[21] = {};
    printf("请输入用户名:");
    scanf("%s",name);
    while(getchar()!='\n');
    printf("请输入密码:");
    scanf("%s",pswd);
    while(getchar()!='\n');
    //打包并发送
    Pack_t* pack =make_package (LOGIN_TYPE,name,pswd);
    send_msg(sockfd,pack,pack->pack_len);
    free(pack);
    pack = NULL;

    pack = recv_msg(sockfd);
    printf("登录%s\n",pack->msg);
    if(strcmp(pack->msg,"Success")==0)
    {
        //进入用户界面
        user_interface(sockfd,name);
        //发送退出消息
        Pack_t* res = make_package(TYPE_END,NULL,"\0");
        send_msg(sockfd,res,res->pack_len);
        free(res);
    }
    else if(strcmp(pack->msg,"Failed")==0)
    {
         free(pack);
    }
}


void regist(int sockfd)
{
    //输入基本信息
    char pswd[21]={},name[21] = {};
    printf("请输入用户名:");
    scanf("%s",name);
    while(getchar()!='\n');
    printf("请输入密码:");
    scanf("%s",pswd);
    while(getchar()!='\n');
    //打包并发送
    Pack_t* pack =make_package (RIGIST_TYPE,name,pswd);
    send_msg(sockfd,pack,pack->pack_len);
    free(pack);
    pack = NULL;

    pack = recv_msg(sockfd);
    printf("注册%s[回车继续]\n",pack->msg);
    while(getchar()!='\n');
    free(pack);
}


void main_interface(int sockfd)
{
    while(1)
    {
        system("clear");
        printf("1.登录 2.注册 0.退出\n");
        int ch = 0;
        scanf("%d",&ch);
        while(getchar()!='\n');
        switch(ch)
        {
            case 1:
                login(sockfd);
                break;
            case 2:
                regist(sockfd);
                break;
            case 0:
                return ;
            default:
                break;
        }
    }
}
