#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "window.h"



//getch函数，获取输入单个字符，并且不用回车
char getch()
{
    int c=0;
    struct termios org_opts, new_opts; 
    tcgetattr(STDIN_FILENO, &org_opts);//获取标准输入的终端属性
    memcpy(&new_opts, &org_opts, sizeof(new_opts));//复制标准输入终端属性
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN); // 禁止回显及规范输入
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts); // 设置为新的终端属性
    c=getchar(); // 读取字符
    tcsetattr(STDIN_FILENO, TCSANOW, &org_opts); // 恢复终端属性
    return c;
}

//子符串输入（无回车）
char* get_string(char *str,int len)
{
    int i = 0; //字符串的下标，同时用于记录已经输入字符的个数
    char c = 0;//当个字符的容器
    while(1)
    {
        c = getch(); //获取单个字符
        if(c==8 || c==127)//判断退格建-擦掉已有字符
        {
            printf("\b \b");//擦除最后一个字符
            fflush(stdout); //刷新输出缓存
            if(i>0) //字符个数减1
            {
                i--;
            }
        }
        else if(c == '\n') //判断回车-结束输入
        {
            if(i==len) //字符输入满
            {
                str[len-1] = '\0';
            }
            else //通常情况
            {
                str[i] = '\0';//结尾加上结束符
            }
            for(int j=0;j<i;j++)
            {
                printf("\b \b");
                fflush(stdout); //刷新输出缓存
            }
            
            break;
        }
        else
        {
            if(i<len)
            {
                str[i] = c; //获取字符存入字符串
                i++; //字符个数加1
                printf("%c",c); //回显
                fflush(stdout); //刷新输出缓存
            }
        }  
    }
    return str;
}



