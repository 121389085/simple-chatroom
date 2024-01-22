#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include "user_list.h"

#define USER_DATA_PATH "data/data.txt"

Un_t* head = NULL;
Un_t* tail = NULL;
//添加用户节点
Un_t* add_node(Un_t data)
{
    Un_t* node = calloc(1,sizeof(Un_t));
    if(node == NULL)
    {
        perror("node add failed");
        return NULL;
    }
    *node = data;
    if(head == NULL)
    {
        head = node;
        tail = node;
    }
    else
    {
        tail ->next = node;
        tail = node;
    }
    return node;
}

//通过用户名查找
Un_t* find_node(char* name)
{
    Un_t* p = head;
    while(p!=NULL)
    {
        if(strcmp(p->name,name)==0)
        {
            break;
        }
        p  = p->next;
    }
    return p;
}

//全部释放
void freeall()
{
    Un_t* p = head;
    Un_t* q = NULL;
    while(p!=NULL)
    {
        q = p;
        p = p->next;
        free(q);
    }
}

void print_list()
{
    Un_t* p = head;
    while(p!=NULL)
    {
        printf("%p %s %s %d\n",p,p->name,p->pswd,p->sockfd);
        p  = p->next;
    }
}

//保存新节点
void save_list(Un_t* node)
{
    int fd = open(USER_DATA_PATH,O_WRONLY|O_APPEND|O_CREAT,0666);
    if(fd < 0)
    {
        perror("保存错误");
        return ;
    }
    write(fd,node->name,21);
    write(fd,node->pswd,21);
    write(fd,"\n",1);
    close(fd);
}

//加载链表
void load_list()
{
    int fd = open(USER_DATA_PATH,O_RDONLY);
    if(fd <0)
    {
        return ;
    }

    Un_t node = {};
    char temp = 0;
    while(1)
    {
        node.sockfd = -1;
        read(fd,node.name,21);
        read(fd,node.pswd,21);
        int bytes = read(fd,&temp,1);
        if(bytes <=0)
        {
            break;
        }
        add_node(node);
        memset(&node,0,sizeof(Un_t));
    }
    close(fd);
}