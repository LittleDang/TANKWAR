#ifndef TW_SOCKET_H__
#define TW_SOCKET_H__
#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
namespace TW {

struct msg {
    unsigned short len;
    std::vector<char> data;
};

msg getMsg(int fd); //不读取到完整的数据报就一直阻塞
void sendMsg(int fd, std::string s); //发送数据

void readn(int fd, void* vptr, size_t n); //辅助函数
class server {
public:
    server(int port);
    ~server();
    void waitClients(void (*funtor)(int fd)); //创建一个线程来等待客户
private:
    int listenfd;
    struct sockaddr_in sockaddr;
    std::vector<int> clients_fd; //存储所有客户的fd
    void waitClients_funtor(void (*funtor)(int fd));
    int closing;
};
class client {
public:
    client(char* serverip, int port);
    ~client();
    void sendMsg(std::string& s);
    msg getMsg();

private:
    int socketfd;
};
}
#endif