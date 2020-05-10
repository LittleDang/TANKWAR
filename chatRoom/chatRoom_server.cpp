#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <memory.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define MAXLINE 1024
using namespace std;
vector<string> client;
vector<string> msg;
int sendIndex = -1;
string key = "helloworld";
int isNewClient(char* buff)
{
    string s(buff);
    string temp_key = s.substr(0, 10); //前7个来验证
    if (temp_key == key) {
        client.push_back(s.substr(11));
        return 1;
    } else {
        return 0;
    }
}
int main(int argc, char** argv)
{
    int listenfd;
    struct sockaddr_in sockaddr;
    char buff[MAXLINE];
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(10004); //本地端口

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bind(listenfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    //将未使用的socket与本地主机ip和端口绑定
    listen(listenfd, 1024);
    //开始监听这个socket，队列长度
    cout << "Please wait for the client information\n";

    for (;;) {
        int connfd; //listen到的socket
        struct sockaddr_in conn_addr;
        socklen_t len = sizeof(conn_addr);
        if ((connfd = accept(listenfd, (struct sockaddr*)(&conn_addr), &len)) == -1) {
            cout << "accpet socket error" << endl;
            continue;
        }
        int n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        if (isNewClient(buff)) {
            string temp = "welcome new client[";
            char* client_addr = inet_ntoa(conn_addr.sin_addr);

            temp = temp + client_addr + ":" + to_string(conn_addr.sin_port) + "] name:" + client.back() + "\n";
            msg.push_back(temp);
            cout << msg[++sendIndex];
        }
        close(connfd);
    }
    close(listenfd);
}