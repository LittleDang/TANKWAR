#include "../include/tw_net.h"
#include "../include/tw_protocol.h"
#include "../include/tw_ui.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
TW::net::client* c;
TW::ui::text_box* text_ptr;
TW::ui::input_box* input_ptr;

vector<string> back_string;
int n;
void show()
{
    do {
        char buff[1024];
        TW::net::msg m = c->getMsg();

        if (m.flag == 0) {
            //text_ptr->add_text("duan kai lian jie");
            break;
        }
        if (m.flag == -1) {
            //text_ptr->add_text("chao shi");
            continue;
        }
        for (int i = 0; i < m.data.size(); i++)
            buff[i] = m.data[i];
        buff[m.len] = '\0';
        text_ptr->add_text(std::string(buff));
    } while (1);
}
int main(int argc, char* argv[])
{
    TW::net::client cli = TW::net::client(argv[1], 10004);
    c = &cli;

    initscr();
    noecho(); //关闭回显模式
    keypad(stdscr, 1); //开启键值转换模式
    curs_set(0);

    TW::ui::text_box textb(1, 1, 80, 40);
    TW::ui::input_box inputb(1, 45, 80, 3);
    text_ptr = &textb;
    input_ptr = &inputb;

    TW::ui::board uiboard;

    uiboard.createKeythread();
    uiboard.addUi(&inputb);
    uiboard.addUi(&textb);

    string data = PROTO_LOGIN + argv[2];
    thread thr(show);
    thr.detach();
    c->sendMsg(data);
    while (1) {
        std::string s = inputb.getText();
        if (!s.empty()) {
            data = PROTO_SPEAK + s;
            c->sendMsg(data);
        }
        uiboard.refresh();
        usleep(10000);
    }

    endwin();
    return 0;
}