#include "../include/tw_socket.h"
#include <iostream>
using namespace std;
TW::server* s;
void callback(int fd)
{
    TW::msg m = TW::getMsg(fd);
    while (1) {
        if (m.data[0] == 'B' && m.data[1] == 'B')
            break;
        for (int i = 0; i < m.data.size(); i++)
            std::cout << m.data[i];
        std::cout << std::endl;
        m = TW::getMsg(fd);
    }
    cout << "bye bye" << endl;
    close(fd);
}

int main(int argc, char* argv[])
{
    s = new TW::server(10004);
    s->waitClients(callback); //创建了一条线程
    sleep(60);
    delete s;
    return 0;
}