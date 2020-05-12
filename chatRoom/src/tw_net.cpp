#include "../include/tw_net.h"
#include <iostream>
#include <thread>
TW::net::server::server(int port)
{
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family
        = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(port); //本地端口

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    //将未使用的socket与本地主机ip和端口绑定
    listen(listenfd, 1024);
    //开始监听这个socket，队列长度
    closing = 0;
}
void TW::net::server::waitClients(void (*funtor)(int fd, clients_thread* info))
{
    std::thread thrd(std::bind(&TW::net::server::waitClients_funtor, this, funtor)); //创建一条线程用来接收新用户
    thrd.detach();
}
uint TW::net::server::add_clients(int fd)
{
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i]->active == 0) { //如果这个客户不在了的话
            clients[i]->active = 1;
            clients[i]->number = i;
            clients[i]->sockect_fd = fd;
            return i;
        }
    }
    clients_thread* temp = new clients_thread;
    temp->active = 1;
    temp->number = clients.size();
    temp->sockect_fd = fd;
    clients.push_back(temp);
    return clients.size() - 1;
}
void TW::net::server::delete_clients(uint id)
{
    //printf("start close thread %d", id);
    if (clients[id]->active == 0) //本来就是没激活状态
    {
        //printf("done\n");
        return;
    }
    clients[id]->active = 0;
    while (clients[id]->active == 0)
        ; //printf("waitting...\n"); //等待直到线程退出，线程退出要吧active置1
    clients[id]->active = 0;
    //printf("done\n");
}
void TW::net::server::clear_allClients()
{
    for (int i = 0; i < clients.size(); i++) {
        delete_clients(i);
        delete clients[i];
    }
}
void TW::net::server::waitClients_funtor(void (*funtor)(int fd, clients_thread* info))
{
    while (!closing) {
        int connfd; //listen到的socket
        if ((connfd = accept(listenfd, NULL, NULL)) == -1) { //这一行会阻塞，知道接收到新的用户为止
            std::cout << "accpet socket error" << std::endl;
            continue;
        }
        uint index = add_clients(connfd);
        std::thread thrd(std::bind(funtor, connfd, clients[index])); //会创建一条线程去处理这个客户
        thrd.detach();
    }
}
int TW::net::readn(int fd, void* vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char* ptr;
    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = recv(fd, ptr, nleft, 0)) <= 0) {
            return nread;
        }
        nleft -= nread;
        ptr += nread;
    }
    return 1;
}

TW::net::msg TW::net::getMsg(int fd) //不读取到完整的数据报就一直阻塞 超时模式则超时返回
{
    u_int16_t len;
    TW::net::msg re;
    int flag = 1;
    if ((flag = recv(fd, &len, 1, 0)) <= 0) {
        re.flag = flag;
        return re;
    }
    if ((flag = recv(fd, ((char*)(&len)) + 1, 1, 0)) <= 0) {
        re.flag = flag;
        return re;
    }
    re.len = len;
    size_t n = len;
    char buff[1024];
    // printf("waitting for %d ch....", (int)n);
    if ((flag = readn(fd, buff, n)) <= 0) {
        re.flag = flag;
        return re;
    };
    //printf("done\n");
    int index = 0;
    while (index < n) {
        re.data.push_back(buff[index++]);
    }
    re.flag = 1;
    return re;
}
TW::net::server::~server()
{
    printf("end...\n");
    clear_allClients();
    printf("done\n");
    close(listenfd);
}

//----------------------------------------------------------------------------
TW::net::client::client(char* serverip, int port)
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
TW::net::client::~client()
{
    close(socketfd);
}
void TW::net::client::sendMsg(std::string& s)
{
    /* std::string ns;
    u_int16_t l = s.length();
    char* lptr = (char*)&l;
    ns.push_back(*lptr);
    ns.push_back(*(lptr + 1));
    for (int i = 0; i < s.length(); i++)
        ns.push_back(s[i]);
    send(socketfd, ns.c_str(), ns.length(), 0);*/
    TW::net::sendMsg(socketfd, s);
}
void TW::net::sendMsg(int fd, std::string s)
{
    std::string ns;
    u_int16_t l = s.length();
    char* lptr = (char*)&l;
    ns.push_back(*lptr);
    ns.push_back(*(lptr + 1));
    for (int i = 0; i < s.length(); i++)
        ns.push_back(s[i]);
    send(fd, ns.c_str(), ns.length(), 0);
}
TW::net::msg TW::net::client::getMsg()
{
    return TW::net::getMsg(socketfd);
}
