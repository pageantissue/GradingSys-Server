#include"server.h"
#include"os.h"
#include<limits>
#include<unistd.h>
#include<cstdio>
#include<cstdlib>
#include<iostream>
using namespace std;

const int Superblock_Start_Addr = 0;     //44B:1block
const int InodeBitmap_Start_Addr = 1 * BLOCK_SIZE; //1024B:2block
const int BlockBitmap_Start_Addr = InodeBitmap_Start_Addr + 2 * BLOCK_SIZE;//10240B:20block
const int Inode_Start_Addr = BlockBitmap_Start_Addr + 20 * BLOCK_SIZE;//120<128: �����x��block
const int Block_Start_Addr = Inode_Start_Addr + INODE_NUM / (BLOCK_SIZE / INODE_SIZE) * BLOCK_SIZE;//32*16=512

const int Disk_Size = Block_Start_Addr + BLOCK_NUM * BLOCK_SIZE;
const int File_Max_Size = 10 * BLOCK_SIZE;


int Root_Dir_Addr;							//��Ŀ¼inode��ַ
int Cur_Dir_Addr;							//��ǰĿ¼:��inode��ַ
char Cur_Dir_Name[310];						//��ǰĿ¼��
char Cur_Host_Name[110];					//��ǰ������
char Cur_User_Name[110];					//��ǰ��½�û���
char Cur_Group_Name[110];					//��ǰ��½�û�����
char Cur_User_Dir_Name[310];				//��ǰ��½�û�Ŀ¼��

int nextUID;								//��һ��Ҫ������û���ʶ��
int nextGID;								//��һ��Ҫ������û����ʶ��

bool isLogin;								//�Ƿ����û���½

FILE* fw;									//��������ļ� д�ļ�ָ��
FILE* fr;									//��������ļ� ���ļ�ָ��
SuperBlock* superblock = new SuperBlock;	//������ָ��
bool inode_bitmap[INODE_NUM];				//inodeλͼ
bool block_bitmap[BLOCK_NUM];				//���̿�λͼ

char buffer[10000000] = { 0 };				//10M������������������ļ�

int count = 0;  //��¼��������

int main()
{
    nextUID = 0;
    nextGID = 0;
    Initialize(); // ��ʼ���ļ�ϵͳ
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;//ipv4
    server_sockaddr.sin_port = htons(MY_PORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server_sock, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    {//�󶨱���ip��˿�
        perror("Bind Failure\n");
        printf("Error: %s\n", strerror(errno));//���������Ϣ
        return -1;
    }
    printf("Listen Port : %d\n", MY_PORT);
    if (listen(server_sock, MAX_QUEUE_NUM) == -1)
    {
        perror("Listen Error");
        close(server_sock);
        return -1;
    }
    
    printf("Waiting for connection!\n");
    while (true)
    {
        Client client;
        client.client_sock = accept(server_sock, (struct sockaddr*)&client.client_addr, &client.length);
        if (client.client_sock == -1)
        {
            perror("Connect Error");
            return -1;
        }
        printf("Connection Successful\n");
        if (fork() == 0) {
            // �ӽ���
            close(server_sock); // �ӽ��̹رշ���������
            Welcome(client);
            handleClient(client); // ����ͻ�������
            close(client.client_sock); // �ӽ��̴�����Ϻ�ر��׽���
            exit(0); // �ӽ��̴�����Ϻ��˳�
        }
        // �����̼�������������Ҫ����Ĵ���
    }
    close(server_sock);//�رշ�������Ӧsocket
    return 0;
}