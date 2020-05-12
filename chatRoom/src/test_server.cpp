#include "../include/tw_net.h"
#include "../include/tw_protocol.h"
#include "../include/tw_user.h"
#include <iostream>
#include <vector>
using namespace std;
TW::net::server* s;
vector<TW::user::account> account_lists;
int check_login(vector<char>& data, string& name)
{
    string s;
    for (auto t : data) {
        s.push_back(t);
    }
    string head = s.substr(0, PROTO_LOGIN.size());
    if (head != PROTO_LOGIN) //验证失败
        return 0;
    name = s.substr(PROTO_LOGIN.size());
    if (name.empty())
        return 0;
    return 1;
}
int check_speak(vector<char>& data, string& sentence)
{
    string s;
    for (auto t : data) {
        s.push_back(t);
    }
    string head = s.substr(0, PROTO_SPEAK.size());
    if (head != PROTO_SPEAK) //验证失败
        return 0;
    sentence = s.substr(PROTO_SPEAK.size());
    if (sentence.empty())
        return 0;
    return 1;
}
void callback(int fd, TW::net::clients_thread* info)
{
    struct timeval tv_timeout;
    tv_timeout.tv_sec = 0;
    tv_timeout.tv_usec = 100000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout)); //设置超时
    TW::user::account n_account;
    TW::net::msg m;
    do {
        m = TW::net::getMsg(fd);
        if (info->active == 0) { //被强行结束
            close(fd);
            info->active = 1;
            return;
        }
    } while (m.flag == -1); //失败一直读取

    if (check_login(m.data, n_account.name) == 0) //验证失败,发送失败信息，然后退出
    {
        TW::net::sendMsg(fd, PROTO_ERROR);
        //info->active = 0;
        close(fd); //关闭这个套接字
        return;
    }
    n_account.socket_fd = fd;
    n_account.thread_index = info->number;
    account_lists.push_back(n_account);

    string s = n_account.name + "-->login in\n";
    cout << s;
    for (int i = 0; i < account_lists.size(); i++) {
        TW::net::sendMsg(account_lists[i].socket_fd, s);
    }
    do {
        m = TW::net::getMsg(fd);
        if (m.flag == 0) {
            //info->active = 0;
            for (auto i = account_lists.begin(); i != account_lists.end(); i++) {
                if (i->thread_index == info->number) {
                    account_lists.erase(i);
                    s = n_account.name + " quit\n";
                    cout << s;
                    for (int i = 0; i < account_lists.size(); i++) {
                        //TW::net::sendMsg(account_lists[i].socket_fd, s);
                    }
                    break;
                }
            }
            break;
        }
        if (m.flag == 1) { //读取成功，解析文本
            string sentence;
            if (check_speak(m.data, sentence) == 1) {
                s = n_account.name + ":" + sentence;
                cout << s << endl; //没有回车不会输出，玄学，博客上有人也遇到这个问题

                for (int i = 0; i < account_lists.size(); i++) {
                    TW::net::sendMsg(account_lists[i].socket_fd, s);
                }
            }
        }
        if (info->active == 0) { //被强行结束
            close(fd);
            cout << "delete thread-->" << n_account.name << endl;
            info->active = 1;

            return;
        }
        usleep(100000);
    } while (1);

    close(fd);
}

int main(int argc, char* argv[])
{
    TW::net::server ser(10004);
    s = &ser;
    s->waitClients(callback); //创建了一条线程
    while (1) {
        sleep(10000);
    }
    return 0;
}