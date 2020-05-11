#include "../include/tw_ui.h"
#include <unistd.h> // usleep()

int main(int argc, char* argv[])
{
    initscr();
    noecho(); //关闭回显模式
    keypad(stdscr, 1); //开启键值转换模式
    curs_set(0);
    TW::text_box textb(1, 1, 80, 40);
    TW::input_box inputb(1, 45, 80, 3);
    TW::board uiboard;
    uiboard.createKeythread();
    uiboard.addUi(&inputb);
    uiboard.addUi(&textb);
    // textb.add_text("welcome to my chat room");
    while (1) {
        std::string s = inputb.getText();
        if (!s.empty())
            textb.add_text(s);
        textb.draw();
        inputb.draw();
        uiboard.refresh();
        usleep(10000);
    }
    endwin();
    return 0;
}