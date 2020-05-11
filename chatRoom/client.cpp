#include <arpa/inet.h> //inet_pton
#include <errno.h> //errno
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> //close
#define MAXLINE 1024
int main(int argc, char** argv)
{
    char* servInetAddr = argv[1];
    int socketfd;
    struct sockaddr_in sockaddr;
    char recvline[MAXLINE], sendline[MAXLINE];
    int n;

    if (argc != 2) {
        printf("client <ipaddress> \n");
        exit(0);
    }

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    //创建一个socket IPV4   字节流 协议（0是默认）
    memset(&sockaddr, 0, sizeof(sockaddr));
    //整个内存块置0
    sockaddr.sin_family = AF_INET; //
    sockaddr.sin_port = htons(10004); //转化到网络字节顺序，高位放低位
    inet_pton(AF_INET, servInetAddr, &sockaddr.sin_addr);
    //将字符串表示的地址转换到二进制
    //IPV4 字符串 二进制输出
    if ((connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0) {
        //建立与指定socket的连接，socketfd是待连接的，sockaddr是存放目标主机的地址和端口
        //成功放回0,失败-1
        printf("connect error %s errno: %d\n", strerror(errno), errno);
        exit(0);
    }

    printf("send message to server\n");

    fgets(sendline, 1024, stdin);

    if ((send(socketfd, sendline, strlen(sendline), 0)) < 0) {
        //向已连接的socket发送字符串
        printf("send mes error: %s errno : %d", strerror(errno), errno);
        exit(0);
    }
    char buff[1024];
    sleep(5);
    int tn = read(socketfd, buff, 30);
    buff[tn] = '\0';
    printf("%s\n", buff);
    close(socketfd); //关闭socket
    printf("exit\n");
    exit(0);
}