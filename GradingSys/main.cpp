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
const int Inode_Start_Addr = BlockBitmap_Start_Addr + 20 * BLOCK_SIZE;//120<128: 换算成x个block
const int Block_Start_Addr = Inode_Start_Addr + INODE_NUM / (BLOCK_SIZE / INODE_SIZE) * BLOCK_SIZE;//32*16=512

const int Disk_Size = Block_Start_Addr + BLOCK_NUM * BLOCK_SIZE;
const int File_Max_Size = 10 * BLOCK_SIZE;


int Root_Dir_Addr;							//根目录inode地址
int Cur_Dir_Addr;							//当前目录:存inode地址
char Cur_Dir_Name[310];						//当前目录名
char Cur_Host_Name[110];					//当前主机名
char Cur_User_Name[110];					//当前登陆用户名
char Cur_Group_Name[110];					//当前登陆用户组名
char Cur_User_Dir_Name[310];				//当前登陆用户目录名

int nextUID;								//下一个要分配的用户标识号
int nextGID;								//下一个要分配的用户组标识号

bool isLogin;								//是否有用户登陆

FILE* fw;									//虚拟磁盘文件 写文件指针
FILE* fr;									//虚拟磁盘文件 读文件指针
SuperBlock* superblock = new SuperBlock;	//超级块指针
bool inode_bitmap[INODE_NUM];				//inode位图
bool block_bitmap[BLOCK_NUM];				//磁盘块位图

char buffer[10000000] = { 0 };				//10M，缓存整个虚拟磁盘文件

int count = 0;  //记录操作次数

int main()
{
    nextUID = 0;
    nextGID = 0;
    Initialize(); // 初始化文件系统
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;//ipv4
    server_sockaddr.sin_port = htons(MY_PORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server_sock, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    {//绑定本地ip与端口
        perror("Bind Failure\n");
        printf("Error: %s\n", strerror(errno));//输出错误信息
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
            // 子进程
            close(server_sock); // 子进程关闭服务器监听
            Welcome(client);
            handleClient(client); // 处理客户端请求
            close(client.client_sock); // 子进程处理完毕后关闭套接字
            exit(0); // 子进程处理完毕后退出
        }
        // 父进程继续监听，不需要额外的处理
    }
    close(server_sock);//关闭服务器响应socket
    return 0;
}