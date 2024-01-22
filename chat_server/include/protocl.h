#ifndef PROTOCL_H_
#define PROTOCL_H_

enum protocl_type
{
    TYPE_START = 0,
    LOGIN_TYPE,
    RIGIST_TYPE,
    PRIVATE_CHAT_TYPE,
    GROUP_CHAT_TYPE,
    FFLUSH_USER,
    TYPE_END
};

typedef struct protocl_pack
{
    int pack_len;//数据包的长度
    enum protocl_type type; //消息的类型
    char name[21]; //用户名
    char msg[1]; //其他字符串信息
}Pack_t;

Pack_t* make_package(int type,char* name,char*msg);

#endif