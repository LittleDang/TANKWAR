#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024
int main(int argc, char** argv)
{
    int listenfd, connfd;
    struct sockaddr_in sockaddr;
    char buff[MAXLINE];
    int n;

    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(10004);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bind(listenfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    //将未使用的socket与本地主机ip和端口绑定
    listen(listenfd, 1024);
    //开始监听这个socket，队列长度
    printf("Please wait for the client information\n");

    for (;;) {
        if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {
            printf("accpet socket error: %s errno :%d\n", strerror(errno), errno);
            continue;
        }

        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        printf("recv msg from client:%s", buff);
        close(connfd);
    }
    close(listenfd);
}