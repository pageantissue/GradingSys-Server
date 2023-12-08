#pragma once
#include<stdio.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>
#include<cerrno>
#include<cstdlib>

#define MY_PORT 6591//端口号
#define BUF_SIZE 1024//最大缓存
#define MAX_QUEUE_NUM 5//最大连接数

struct Client //服务端客户
{
    int client_sock;

    int Cur_Dir_Addr;							//当前目录:存inode地址
    char Cur_Dir_Name[310];						//当前目录名
    char Cur_User_Name[110];					//当前登陆用户名
    char Cur_Group_Name[110];					//当前登陆用户组名
    char Cur_User_Dir_Name[310];				//当前登陆用户目录名

    char buffer[BUF_SIZE]; //缓存用户的输入
    struct sockaddr_in client_addr;//保存客户端地址信息
    socklen_t length = sizeof(client_addr);//需要的内存大小
};

void Welcome(Client&);
void handleClient(Client&);
void localize(Client&); //关键变量局部化
void globalize(Client&);
int Initialize();