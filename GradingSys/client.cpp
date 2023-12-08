#include "client.h"

int run_client()
{
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);//建立客户端socket

    struct sockaddr_in servaddr;//保存服务器端地址信息
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;//ipv4协议
    servaddr.sin_port = htons(MY_PORT);//端口
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);//ip地址

    printf("connect to %s:%d\n", SERVER_IP, MY_PORT);
    int connect_fd = connect(client_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));//建立连接
    if (connect_fd < 0) {
        perror("Connect Error");
        _exit(1);
    }
    printf("Connect Successful\n");
    char sendbuf[BUF_SIZE];
    char recvbuf[BUF_SIZE];
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {//数据传送
        memset(recvbuf, 0, sizeof(recvbuf));
        printf("send message:%s", sendbuf);
        send(client_sockfd, sendbuf, strlen(sendbuf), 0);
        if (strcmp(sendbuf, "exit\n") == 0) break;
        ssize_t len = recv(client_sockfd, recvbuf, sizeof(recvbuf), 0);
        if (len <= 0) {
            printf("receive failure");
            break;
        }
        printf("recv message:%s\n", recvbuf);
        memset(sendbuf, 0, sizeof(sendbuf));
    }
    close(client_sockfd);//关闭客户端socket
    return 0;
}
