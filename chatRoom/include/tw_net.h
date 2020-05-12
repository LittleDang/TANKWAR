#ifndef TW_NET_H__
#define TW_NET_H__
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
namespace net {
    struct msg {
        ushort len;
        int flag; //-1超时 0连接断开 1成功
        std::vector<char> data;
    };
    struct clients_thread {
        int sockect_fd; //记录了与之对应的socket
        int active; //1表示活跃，0表示关闭线程
        uint number; //每个客户唯一的标志数字,通过这个来区分客户
    };
    msg getMsg(int fd); //不读取到完整的数据报就一直阻塞 超时模式则超时返回
    void sendMsg(int fd, std::string s); //发送数据

    int readn(int fd, void* vptr, size_t n); //辅助函数
    class server {
    public:
        server(int port);
        ~server();
        void waitClients(void (*funtor)(int fd, clients_thread* info)); //创建一个线程来等待客户
        //socket 存储是否要关闭 自定义的参数列表
    private:
        int listenfd;
        struct sockaddr_in sockaddr;
        std::vector<clients_thread*> clients; //存储所有客户的fd
        uint add_clients(int fd);
        void delete_clients(uint id);
        void clear_allClients();
        void waitClients_funtor(void (*funtor)(int fd, clients_thread* info));
        int closing;
    };
    class client {
    public:
        client(char* serverip, int port);
        ~client();
        void sendMsg(std::string& s);
        msg getMsg();
        int socketfd; //为了调试方便而。。
    private:
    };
}
}
#endif