#include "../include/tw_socket.h"
#include <iostream>
#include <thread>
TW::server::server(int port)
{
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(port); //本地端口

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    //将未使用的socket与本地主机ip和端口绑定
    listen(listenfd, 1024);
    //开始监听这个socket，队列长度
    closing = 0;
}
void TW::server::waitClients(void (*funtor)(int fd))
{
    std::thread thrd(std::bind(&TW::server::waitClients_funtor, this, funtor));
    thrd.detach();
}
void TW::server::waitClients_funtor(void (*funtor)(int fd))
{
    while (!closing) {
        int connfd; //listen到的socket
        if ((connfd = accept(listenfd, NULL, NULL)) == -1) { //这一行会阻塞，知道接收到新的用户为止
            std::cout << "accpet socket error" << std::endl;
            continue;
        }
        std::thread thrd(std::bind(funtor, connfd)); //会创建一条线程去处理这个客户
        thrd.detach();
    }
}
void TW::readn(int fd, void* vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char* ptr;

    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = recv(fd, ptr, nleft, 0)) < 0) {
            if (errno == EINTR)
                nread = 0; /* and call read() again */
            else
                return;
        } else if (nread == 0)
            break; /* EOF */

        nleft -= nread;
        ptr += nread;
    }
}

TW::msg TW::getMsg(int fd) //不读取到完整的数据报就一直阻塞
{
    u_int16_t len;
    recv(fd, &len, 1, 0);
    recv(fd, ((char*)(&len)) + 1, 1, 0);
    TW::msg re;
    re.len = len;
    size_t n = len;
    char buff[1024];
    readn(fd, buff, n);
    int index = 0;
    while (index < n) {
        re.data.push_back(buff[index++]);
    }
    return re;
}
TW::server::~server()
{
    close(listenfd);
}
TW::client::client(char* serverip, int port)
{
    char* servInetAddr = serverip;
    struct sockaddr_in sockaddr;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    //创建一个socket IPV4   字节流 协议（0是默认）
    memset(&sockaddr, 0, sizeof(sockaddr));
    //整个内存块置0
    sockaddr.sin_family = AF_INET; //
    sockaddr.sin_port = htons(port); //转化到网络字节顺序，高位放低位
    inet_pton(AF_INET, servInetAddr, &sockaddr.sin_addr);
    //将字符串表示的地址转换到二进制
    //IPV4 字符串 二进制输出
    if ((connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0) {
        //建立与指定socket的连接，socketfd是待连接的，sockaddr是存放目标主机的地址和端口
        //成功放回0,失败-1
        std::cout << "connect error %s errno" << std::endl;
    }
}
TW::client::~client()
{
    close(socketfd);
}
void TW::client::sendMsg(std::string& s)
{
    std::string ns;
    u_int16_t l = s.length();
    char* lptr = (char*)&l;
    ns.push_back(*lptr);
    ns.push_back(*(lptr + 1));
    for (int i = 0; i < s.length(); i++)
        ns.push_back(s[i]);
    TW::sendMsg(socketfd, ns);
}
void TW::sendMsg(int fd, std::string s)
{
    send(fd, s.c_str(), s.length(), 0);
}
TW::msg TW::client::getMsg()
{
    return TW::getMsg(socketfd);
}
