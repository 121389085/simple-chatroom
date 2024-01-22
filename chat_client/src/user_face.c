#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "client.h"
#include "protocl.h"
#include "window.h"

int flag = 1;
char (*user_list)[21] = NULL;
int len = 0;
pthread_mutex_t mutex;
char *user_path = NULL;
char chat_mode[21] = {};//正在进行的聊天模式-用户名直接作为模式
char *input_memory = NULL;//输入缓存

void get_user_list(int sockfd)
{
    len = 0;
    while(1)
    {
        Pack_t* pack = recv_msg(sockfd);
        if(pack==NULL)
        {
            //printf("NULL\n");
            break ;
        }
        if(pack->msg[0] == '0')
        {
            //printf("0");
            break;
        }
        //printf("%s\n",pack->name);
        strcpy(user_list[len],pack->name);
        len++;
    }
    pthread_mutex_unlock(&mutex);
}

void show_history(char* name,char *title)
{
    /*获取文件路径*/
    char path[128] = {};
    strcpy(path,user_path);
    strcat(path,name);

    int fd = open(path,O_RDONLY);
    if(fd>=0)
    {
        //显示聊天记录
        while(1)
        {
            char buf[128] = {};
            int res = read(fd,buf,128);
            if (res <=0)
            {
                break;
            }
            printf("%s",buf);
        }
    }
    //显示输入的部分
    printf("==================%s==================\n",title);
    printf("请输入消息[输入\'/exit\'返回上一级]:%s",input_memory);
    fflush(stdout);
}

//私聊类型消息处理
void private_msg(Pack_t* pack)
{
    /*获取文件路径*/
    char path[128] = {};
    strcpy(path,user_path);
    strcat(path,pack->name);
    //记录存储
    int fd = open(path,O_WRONLY|O_APPEND|O_CREAT,0666);
    write(fd,pack->name,strlen(pack->name));
    write(fd,":",1);
    write(fd,pack->msg,strlen(pack->msg));
    write(fd,"\n",1);
    close(fd);

    if(strcmp(chat_mode,pack->name)==0)
    {   
        system("clear");
        char title[64] = "chat with ";
        strcat(title,pack->name);
        show_history(pack->name,title);
    }
}

//群聊消息处理
void group_msg(Pack_t* pack)
{
    /*获取文件路径*/
    char path[128] = {};
    strcpy(path,user_path);
    strcat(path,"group");
    //记录存储
    int fd = open(path,O_WRONLY|O_APPEND|O_CREAT,0666);
    write(fd,pack->name,strlen(pack->name));
    write(fd,":",1);
    write(fd,pack->msg,strlen(pack->msg));
    write(fd,"\n",1);
    close(fd);

    if(strcmp(chat_mode,"group")==0)
    {
        int fd = open(path,O_RDONLY);
        if(fd>=0)
        {
           system("clear");
            show_history("group","group chat");
        }
    }
}

//消息接收处理线程函数（登录后消息接收的唯一入口)
void user_msg_recv(int sockfd)
{
    while(1)
    {
        Pack_t* res = recv_msg(sockfd);
        switch (res->type)
        { 
            case PRIVATE_CHAT_TYPE:
                private_msg(res);
                break;
            case GROUP_CHAT_TYPE:
                group_msg(res);
                break;
            case FFLUSH_USER:
                //printf("列表刷新\n");
                get_user_list(sockfd);
                break;
            case TYPE_END: //强制登出
                flag = 0;
                system("clear");
                printf("%s[输入回车继续]",res->msg);
                fflush(stdout);
                return;
            default:
                break;
        }
        free(res);
    }
}


//私聊操作
void private_chat(char* name,int sockfd)
{
    while(flag)
    {
        system("clear");
        printf("=============choose user============\n");
        Pack_t pack = {};
        pack.type = FFLUSH_USER;
        send_msg(sockfd,&pack,sizeof(pack));
        pthread_mutex_lock(&mutex);
        /*获取当前的在线用户*/
        for(int i=0;i<len;i++)
        {
            printf("%d.%s\n",i+1,user_list[i]);
        }
        printf("0.返回上一级\n");
        char ch[3] = {};
        scanf("%s",ch);
        while(getchar()!='\n');
        if(flag == 0 || ch[0] == '0')
        {
            break;
        }
        /*获取选择的用户名*/
        int num = atoi(ch);
        char* user_name = user_list[num-1];
        strcpy(chat_mode,user_name);
        char path[128] = {};
        strcpy(path,user_path);
        strcat(path,user_name);
        int fd = open(path,O_WRONLY|O_APPEND|O_CREAT,0666);
        while(flag)
        {
            system("clear");
            char title[64] = "chat with ";
            strcat(title,user_name);
            show_history(user_name,title);
            get_string(input_memory,127);
            write(fd,"\t\t\t\t\t",5*strlen("\t"));
            write(fd,name,strlen(name));
            write(fd,":",1);
            write(fd,input_memory,strlen(input_memory));
            write(fd,"\n",1);
            if(strcmp(input_memory,"/exit") == 0 || flag == 0)
            {
                break;
            }
            //打包发送
            Pack_t* pack = make_package(PRIVATE_CHAT_TYPE,user_name,input_memory);
            send_msg(sockfd,pack,pack->pack_len);
            free(pack);
            memset(input_memory,0,128);
        }
        close(fd);
        memset(input_memory,0,128);
        memset(chat_mode,0,21);
    }
}

//群聊操作
void group_chat(int sockfd)
{
    strcpy(chat_mode,"group");
    while(1)
    {
        system("clear");
        show_history("group","group chat");
        get_string(input_memory,127);

        if(strcmp(input_memory,"/exit") == 0 || flag == 0)
        {
            break;
        }
        //打包发送
        Pack_t* res = make_package(GROUP_CHAT_TYPE,NULL,input_memory);
        send_msg(sockfd,res,res->pack_len);
        free(res);
        memset(input_memory,0,128);
    }
    memset(input_memory,0,128);
    memset(chat_mode,0,21);
}

//获取当前用户的记录的根目录
void make_data_dir(char* name)
{
    sprintf(user_path,"./data/%s/",name);
    if(access(user_path,F_OK)<0) //如果没有现成的
    {
        mkdir(user_path,0775); //创建目录
    }
}

void user_interface(int sockfd,char* name)
{
    char up[128] = {}; //用户目录的容器
    char ul[128][21] = {};  //用户列表的容器
    char im[128] = {};//输入缓存容器
    user_list = ul;
    user_path = up;
    input_memory = im;
    make_data_dir(name); //根据用户获取目录
    flag = 1;
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_lock(&mutex);
    pthread_t tid;
    pthread_create(&tid,NULL,(void*)user_msg_recv,(void*)(__intptr_t)sockfd);
    while(flag)
    {
        system("clear");
        printf("1.私聊 2群聊 0.退出登录");
        char ch = 0;
        ch = getchar();
        if(ch != '\n')
        {
            while(getchar()!='\n');
        }
        if(flag == 0)
        {
            printf("退出登录\n");
            break;
        }
        switch (ch)
        {
        case '1':
            private_chat(name,sockfd);
            break;
        case '2':
            group_chat(sockfd);
            break;
        case '0':
            pthread_cancel(tid);
            pthread_join(tid,NULL);
            return ;
        default:
            break;
        }
    }
}