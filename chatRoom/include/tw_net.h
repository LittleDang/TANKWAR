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
    struct msg { //接收数据存储的一个数据结构，
        ushort len; //数据长度
        int flag; //接收情况，-1超时 0连接断开 1成功
        std::vector<char> data;
    };
    struct clients_thread { //记录了客户对应的线程的数据结构，每个客户对应一个线程
        int sockect_fd; //记录了与之对应的socket
        int active; //1表示活跃，0表示关闭线程
        uint number; //每个客户唯一的标志数字,通过这个来区分客户
    };
    msg getMsg(int fd); //通过socket读取自定义的一帧数据
    //阻塞模式：不读取到完整的数据报就一直阻塞
    //超时模式：超时则超时返回
    void sendMsg(int fd, std::string s); //发送数据

    int readn(int fd, void* vptr, size_t n); //辅助函数，读取n个字节的msg
    //阻塞模式：不读取到完整的数据报就一直阻塞
    //超时模式：超时则超时返回
    class server {
    public:
        server(int port);
        ~server();
        void waitClients(void (*funtor)(int fd, clients_thread* info)); //创建一个线程来等待客户
        //socket 存储对应其管理的线程的数据
        //如果从内部推出，则置info->active=0
        //如果从外部推出，则是functor内部自己负责检测info->active是否为0,
        //为0表示外部发出了退出指令，处理好数据后，从内部置1表示线程退出完毕
    private:
        int listenfd; //用来监听本地端口的socket
        struct sockaddr_in sockaddr;
        std::vector<clients_thread*> clients; //存储所有客户的信息，包括唯一对应的number,socket,以及状态
        uint add_clients(int fd); //添加一个新的用户，自动创建一个唯一的number并且添加到clients
        void delete_clients(uint id); //根据number删除客户，并且等待线程退出完毕才返回
        void clear_allClients(); //清空所有的客户，析构函数会自动调用
        void waitClients_funtor(void (*funtor)(int fd, clients_thread* info)); //用来辅助waitClients的一个监听客户线程
        int closing; //用来标志关闭监听线程的，没写
    };
    class client {
    public:
        client(char* serverip, int port); //创建与服务器的连接
        //服务器Ip 端口
        ~client();
        void sendMsg(std::string& s);
        //发送数据给服务器
        msg getMsg();
        //从服务器读取数据
        int socketfd; //为了调试方便而。。
    private:
    };
}
}
#endif