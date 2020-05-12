#include "../include/tw_net.h"
#include <iostream>
using namespace std;
TW::net::server* s;
void callback(int fd)
{
    TW::net::msg m = TW::net::getMsg(fd);
    while (1) {
        if (m.data[0] == 'B' && m.data[1] == 'B')
            break;
        for (int i = 0; i < m.data.size(); i++)
            std::cout << m.data[i];
        std::cout << std::endl;
        m = TW::net::getMsg(fd);
    }
    cout << "bye bye" << endl;
    close(fd);
}

int main(int argc, char* argv[])
{
    s = new TW::net::server(10004);
    s->waitClients(callback); //创建了一条线程
    sleep(60);
    delete s;
    return 0;
}