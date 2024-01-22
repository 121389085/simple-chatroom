#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "user_list.h"
#include "server.h"
#include "control.h"

pthread_mutex_t mutex;

int clientfd=0;

void signal_handler(int signo)
{
    if(clientfd>0)
    {
        Pack_t* pack = make_package(TYPE_END,NULL,"服务器已断开");
        send_msg(clientfd,pack,pack->pack_len);
        free(pack);
    }
    freeall();
    pthread_mutex_destroy(&mutex);
    printf("\n服务器进程已退出,内存已释放\n");
    exit(1);
}

int main()
{
    signal(SIGINT,signal_handler);
    load_list();
   int listenfd =  listen_start(8888);
   pthread_mutex_init(&mutex,NULL);
   while(1)
   {
        clientfd = accpet_client(listenfd);
        if(clientfd >=0)
        {
            pthread_t tid;
            pthread_create(&tid,NULL,(void*)client_msg_control,(void*)(__intptr_t)clientfd);
        }
   }
    close(listenfd);
    return 0;
}