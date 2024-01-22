#ifndef USER_LIST_H
#define USER_LIST_H


//用户链表的结构体
typedef struct user_node
{
    char name[21];
    char pswd[21];
    int sockfd;
    struct user_node* next;
}Un_t;

Un_t* add_node(Un_t data);
Un_t* find_node(char* name);
void print_list();
void freeall();
void save_list(Un_t *node);
void load_list();

#endif