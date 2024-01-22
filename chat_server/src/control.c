#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "server.h"
#include "user_list.h"
#include "protocl.h"

extern Un_t* head;
extern pthread_mutex_t mutex;

//注册
void regist(int sockfd,Pack_t*pack)
{
    pthread_mutex_lock(&mutex);
    //检查重名
    Un_t*p =  find_node(pack->name);
    if(p == NULL)
    {
        Pack_t* res = make_package(1,NULL,"Success");
        send_msg(sockfd,res,res->pack_len);
        free(res);
        Un_t data = {};
        strcpy(data.name,pack->name);
        strcpy(data.pswd,pack->msg);
        Un_t* node = add_node(data);
        if(node !=NULL)
        {
            save_list(node);
        }
    }
    else
    {
        Pack_t* res = make_package(0,NULL,"Failed");
        send_msg(sockfd,res,res->pack_len);
        free(res);
    }
    pthread_mutex_unlock(&mutex);
}


//登录
Un_t* login(int sockfd,Pack_t* pack)
{
    Un_t* p = find_node(pack->name);
    if(p == NULL)
    {
        Pack_t* res = make_package(0,NULL,"Failed");
        send_msg(sockfd,res,res->pack_len);
        free(res);
        return NULL;
    }
    else if(strcmp(p->pswd,pack->msg)!=0)
    {
        Pack_t* res = make_package(0,NULL,"Failed");
        send_msg(sockfd,res,res->pack_len);
        free(res);
        return NULL;
    }
    else
    {
         //设置用户对应的套接字
        if(p->sockfd <0)
        {
            printf("[%p]设置用户%s套接字,原:%d",p,p->name,p->sockfd);
            p->sockfd = sockfd;
            printf("新:%d\n",p->sockfd);
        }
        else //重复登录的处理
        {
            Pack_t* res = make_package(TYPE_END,NULL,"用户在异地登录");
            send_msg(p->sockfd,res,res->pack_len);
            p->sockfd = sockfd;
        }
        Pack_t* res = make_package(0,NULL,"Success");
        send_msg(sockfd,res,res->pack_len);
        free(res);    
        return p;
    }
}


//发送在线用户列表
void send_online_user(int sockfd)
{
    Un_t* p = head;
    Pack_t* pack = make_package(FFLUSH_USER,NULL,"1");
    send_msg(sockfd,pack,pack->pack_len);
    while(p!=NULL)
    {
        if(p->sockfd>=0)
        {
            strcpy(pack->name,p->name);
           // printf("%s\n",pack->name);
            send_msg(sockfd,pack,pack->pack_len);
        }
        p = p->next;
    }
    strcpy(pack->msg,"0");
    send_msg(sockfd,pack,pack->pack_len);
    free(pack);
}


//私聊信息处理
void private_chat(Un_t* p,Pack_t *pack)
{
    Un_t * t = find_node(pack->name); //找到目标用户
    strcpy(pack->name,p->name);
    send_msg(t->sockfd,pack,pack->pack_len); //转发消息

}


//群聊消息处理
void group_chat(Un_t* p,Pack_t* pack)
{
    Un_t* q = head;
    strcpy(pack->name,p->name);
    while(q!=NULL)
    {
        if(q->sockfd >= 0)
        {
            send_msg(q->sockfd,pack,pack->pack_len);
        }
        q = q->next;
    }
}


//用户信息接收控制
void client_msg_control(int sockfd)
{
    Un_t* p_user = NULL;
    while(1)
    {
        Pack_t* pack = recv_msg(sockfd);
        if(pack == NULL)
        {
            break;
        }
        switch(pack->type)
        {
            case RIGIST_TYPE:
                regist(sockfd,pack);
                break;
            case LOGIN_TYPE:
                p_user = login(sockfd,pack);
                break;
            case FFLUSH_USER:
                send_online_user(sockfd);
                break;
            case PRIVATE_CHAT_TYPE:
                private_chat(p_user,pack);
                break;
            case GROUP_CHAT_TYPE:
                group_chat(p_user,pack);
                break;
            case TYPE_END: 
                if(p_user != NULL)
                {
                    if(p_user->sockfd == sockfd) //不是因为重复登录被迫下线时
                    {
                        p_user ->sockfd = -1; //将用户变为下线模式
                    }   
                    p_user = NULL;
                }
                break;
            default:
                break;
        }
        free(pack);
        print_list();
    }
     if(p_user != NULL)
    {
        p_user ->sockfd = -1;
        p_user = NULL;
    }

    close(sockfd);
}
