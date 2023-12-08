#include"server.h"
#include"os.h"
#include<limits>
#include<unistd.h>
#include<cstdio>
#include<cstdlib>
#include<iostream>
extern int count;

void Welcome(Client& client)
{
    int client_sock = client.client_sock;
    char buff[] = "GradingSys Greeting!\n";
    send(client_sock, buff, strlen(buff), 0);
}
int Initialize()
{
    //###############打不开文件################
    if ((fr = fopen(GRADE_SYS_NAME, "rb")) == NULL)
    {
        fw = fopen(GRADE_SYS_NAME, "wb");
        if (fw == NULL) {
            printf("Failed to open the virtual disc file!\n");
            return 0;
        }
        fr = fopen(GRADE_SYS_NAME, "rb");
        printf("Virtual disc file openned successfully!\n");
        //初始化变量
        isLogin = false;
        strcpy(Cur_User_Name, "root");
        strcpy(Cur_Group_Name, "root");

        //获取主机名
        memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
        if (gethostname(Cur_Host_Name, sizeof(Cur_Host_Name)) != 0) {
            perror("Error getting hostname");
            return 1;
        }

        Root_Dir_Addr = Inode_Start_Addr;
        Cur_Dir_Addr = Root_Dir_Addr;
        strcpy(Cur_Dir_Name, "/");
        printf("Formatting the file system...\n");

        //系统格式化
        if (!Format()) {
            printf("Formatting file system failed!\n");
            return 0;
        }
        printf("Formatting done.\n\n");
        //Install
        if (!Install()) {

            printf("File system installation failure!\n");
            return 0;
        }
    }
    else
    {
        fw = fopen(GRADE_SYS_NAME, "rb+"); //在原来的基础上修改文件
        if (fw == NULL) {
            printf("Disk files openning failure!\n");
            return false;
        }
        //初始化变量
        isLogin = false;
        strcpy(Cur_User_Name, "root");
        strcpy(Cur_Group_Name, "root");

        //获取主机名
        memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
        if (gethostname(Cur_Host_Name, sizeof(Cur_Host_Name)) != 0) {
            perror("Error getting hostname");
            return 1;
        }

        //获取根目录
        Root_Dir_Addr = Inode_Start_Addr;
        Cur_Dir_Addr = Root_Dir_Addr;
        strcpy(Cur_Dir_Name, "/");

        //是否需要格式化
        printf("Format the file system? [y/n]\n");
        char str;
        scanf("%s", &str);
        if (str == 'y') {
            if (!Format()) {
                printf( "Failed to format the system!\n");
                return 0;
            }
            printf("Format done!\n");
        }
        

        //Install
        if (!Install()) {
            printf("File system installation failed!\n");
            return 0;
        }
        printf("File system installation done.\n");
    }
}

void localize(Client& client)
{
    // 全局变量局部化 
    client.Cur_Dir_Addr = Cur_Dir_Addr;
    strcpy(client.Cur_Dir_Name, Cur_Dir_Name);
    strcpy(client.Cur_Group_Name, Cur_Group_Name);
    strcpy(client.Cur_User_Dir_Name, Cur_User_Dir_Name);
    strcpy(client.Cur_User_Name, Cur_User_Name);
}

void globalize(Client& client)
{
    // 全局变量局部化 
    Cur_Dir_Addr = client.Cur_Dir_Addr;
    strcpy(Cur_Dir_Name, client.Cur_Dir_Name);
    strcpy(Cur_Group_Name, client.Cur_Group_Name);
    strcpy(Cur_User_Dir_Name, client.Cur_User_Dir_Name);
    strcpy(Cur_User_Name, client.Cur_User_Name);
}

void handleClient(Client& client)
{
    int client_sock = client.client_sock;
    while (1)
    {
        if (isLogin)
        {
            char* p;
            count++;
            if ((p = strstr(client.Cur_Dir_Name, client.Cur_User_Dir_Name)) == NULL)	//当前是否在用户目录下
            {
                char output_buffer[BUF_SIZE];
                // 使用snprintf将格式化的字符串存储到output_buffer中
                snprintf(output_buffer, BUF_SIZE, "[%s@%s %s]# ", Cur_Host_Name, client.Cur_User_Name, client.Cur_Dir_Name);
                //[Linux@yhl /etc]
                send(client_sock, output_buffer, strlen(output_buffer), 0);
            }
            else
            {
                char output_buffer[BUF_SIZE];
                snprintf(output_buffer, BUF_SIZE, "[%s@%s %s]# ", Cur_Host_Name, client.Cur_User_Name, client.Cur_Dir_Name + strlen(client.Cur_User_Dir_Name));
                //[Linux@yhl ~/app]
                send(client_sock, output_buffer, strlen(output_buffer), 0);
            }
            // 准备接收用户输入
            memset(client.buffer, 0, sizeof(client.buffer));
            int len = recv(client_sock, client.buffer, sizeof(client.buffer), 0);
            if (strcmp(client.buffer, "exit\n") == 0 || len <= 0)
            {
                printf("Client %d has logged out the system!\n", client_sock);
                break;
            }
            cmd(client, count);
        }
        else
        {
            char buff[] = "Welcome to GradingSysOS! Login first, please!\n";
            send(client_sock, buff, strlen(buff), 0);
            while (!login(client));	//登陆
            strcpy(buff, "Successfully logged into our system!\n");
            send(client_sock, buff, strlen(buff), 0);
            //fclose(fw);		//释放文件指针
            //fclose(fr);		//释放文件指针
        }
    }
}