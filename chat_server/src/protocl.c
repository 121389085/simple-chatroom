#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "protocl.h"

//生成协议数据包
Pack_t* make_package(int type,char* name,char*msg)
{
    int pack_len = strlen(msg)+sizeof(Pack_t); //包的实际长度
    Pack_t* package = calloc(1,pack_len); //声明空间
    package->type = type; //设置类型
    if(name != NULL)
    {
        strcpy(package->name,name); //设置用户名
    }
    strcpy(package->msg,msg); //设置消息内容
    package->pack_len =pack_len; //设置包的长度
    return package; //返回包的首地址
}