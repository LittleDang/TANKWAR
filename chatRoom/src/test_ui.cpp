#include "../include/tw_ui.h"
#include <unistd.h> // usleep()

int main(int argc, char* argv[])
{
    initscr();
    noecho(); //关闭回显模式
    keypad(stdscr, 1); //开启键值转换模式
    curs_set(0);
    TW::ui::text_box textb(90, 1, 20, 40);
    TW::ui::input_box inputb(1, 45, 80, 3);
    TW::ui::draw_box drawb(1, 1, 40, 40); //宽度一个顶2

    TW::ui::board uiboard;
    uiboard.createKeythread();
    uiboard.addUi(&inputb);
    uiboard.addUi(&textb);
    uiboard.addUi(&drawb);
    for (int i = 0; i < 40; i++)
        for (int j = 0; j < 40; j++) {
            if (i == j)
                drawb.setPixel(i, j, "\\\\");
            else if (i < j)
                drawb.setPixel(i, j, "00");
            else if (i > j)
                drawb.setPixel(i, j, "##");
        }
    // textb.add_text("welcome to my chat room");
    while (1) {
        std::string s = inputb.getText();
        if (!s.empty())
            textb.add_text(s);
        uiboard.refresh();
        usleep(10000);
    }
    endwin();
    return 0;
}