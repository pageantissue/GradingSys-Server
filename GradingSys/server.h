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

#define MY_PORT 6591//�˿ں�
#define BUF_SIZE 1024//��󻺴�
#define MAX_QUEUE_NUM 5//���������

struct Client //����˿ͻ�
{
    int client_sock;

    int Cur_Dir_Addr;							//��ǰĿ¼:��inode��ַ
    char Cur_Dir_Name[310];						//��ǰĿ¼��
    char Cur_User_Name[110];					//��ǰ��½�û���
    char Cur_Group_Name[110];					//��ǰ��½�û�����
    char Cur_User_Dir_Name[310];				//��ǰ��½�û�Ŀ¼��

    char buffer[BUF_SIZE]; //�����û�������
    struct sockaddr_in client_addr;//����ͻ��˵�ַ��Ϣ
    socklen_t length = sizeof(client_addr);//��Ҫ���ڴ��С
};

void Welcome(Client&);
void handleClient(Client&);
void localize(Client&); //�ؼ������ֲ���
void globalize(Client&);
int Initialize();