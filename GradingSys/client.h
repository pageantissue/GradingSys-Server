#pragma once
#include "server.h"
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cerrno>

#define MY_PORT 1234
#define BUF_SIZE 1024
#define SERVER_IP "127.0.0.1"//�����ip

struct Client //����˵Ŀͻ�
{
    int client_sock;
    char buffer[BUF_SIZE]; //�����û�������
    struct sockaddr_in client_addr;//����ͻ��˵�ַ��Ϣ
    socklen_t length = sizeof(client_addr);//��Ҫ���ڴ��С
};
