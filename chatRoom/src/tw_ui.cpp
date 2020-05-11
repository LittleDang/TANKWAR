#include "../include/tw_ui.h"
#include <thread>
#include <unistd.h>
TW::box_ui::box_ui(int x_, int y_, int w_, int h_)
{
    x = x_;
    y = y_;
    h = h_;
    w = w_;
    win = newwin(h_, w_, y_, x_);
    selected = 0;
}
TW::box_ui::~box_ui()
{
    delwin(win);
}

void TW::box_ui::setPos(int x_, int y_)
{
    x = x_;
    y = y_;
    delwin(win);
    win = newwin(h, w, y, x);
}
void TW::box_ui::setSize(int w_, int h_)
{
    w = w_;
    h = h_;
    delwin(win);
    win = newwin(h, w, y, x);
}

void TW::box_ui::setSelected(int f)
{
    selected = f;
}
void TW::box_ui::setKey(char ch)
{
    buff.push_back(ch);
    if (buff.size() > 1024) //缓存上限1024char 超过自己清除。
        buff.pop_front();
}

//--------------------box

TW::text_box::text_box(int x_, int y_, int w_, int h_)
    : box_ui(x_, y_, w_, h_)
{
}

void TW::text_box::add_text(std::string s)
{
    text.push_back(s);
}

void TW::text_box::clear_text(std::string)
{
    text.clear();
}

void TW::text_box::draw()
{
    wclear(win);
    char tt[2];
    tt[1] = '\0';
    for (int i = 0; i < h - 2; i++) {
        int y = text.size(); //to int
        y = y - i - 1;
        if (y < 0)
            break;
        for (int j = 0; j < text[y].size(); j++) {
            tt[0] = text[y][j];
            mvwprintw(win, h - i - 2, j + 1, "%s", tt);
        }
    }
    if (selected)
        box(win, '|', '-');
    else
        box(win, ' ', ' ');
    //wrefresh(win);
}

//-------------------------text box

TW::input_box::input_box(int x_, int y_, int w_, int h_)
    : box_ui(x_, y_, w_, h_)
{
}
int isVaildCh(char ch)
{
    /*if (ch >= 'a' && ch <= 'z')
        return 1;
    if (ch >= 'A' && ch <= 'Z')
        return 1;
    if (ch >= '0' && ch <= '9')
        return 1;
    if (ch == ' ')
        return 1;*/
    return 1;
}
void TW::input_box::draw()
{
    if (!selected) {
        wclear(win);
        if (!showText.empty())
            mvwprintw(win, 1, 1, "%s", showText.c_str());
        box(win, ' ', ' ');
        //wrefresh(win);
    } else {
        char ch;
        while (!buff.empty()) {
            ch = buff.front();
            buff.pop_front();
            if (ch == '\n') //回车，表示输入结束
            {
                showText.push_back('\0');
                output_text.push_back(showText);
                showText.clear();
            } else if (ch == 7) { //退格键
                if (!showText.empty())
                    showText.pop_back();
            } else if (isVaildCh(ch)) {
                showText.push_back(ch);
            }
        }
        wclear(win);
        if (!showText.empty()) {
            showText.push_back('\0'); //加个结束符表示结束
            mvwprintw(win, 1, 1, "%s", showText.c_str());
            showText.pop_back();
        }
        box(win, '|', '-');
    }
}

std::string TW::input_box::getText()
{
    if (output_text.empty())
        return "";
    std::string s = output_text.front();
    output_text.pop_front();
    return s;
}
//----------------------inputbox

TW::board::board()
{
    selected_ui = NULL;
}
void TW::board::keyFuntor()
{
    while (1) {
        char ch = getch();
        if (ch != 9) {
            if (selected_ui) {
                selected_ui->setKey(ch);
            }
        } else {
            selectNext();
        }
        usleep(10000);
    }
}
void TW::board::createKeythread()
{
    std::thread thr(std::bind(&TW::board::keyFuntor, this));
    thr.detach();
}

void TW::board::addUi(box_ui* u)
{
    if (selected_ui == NULL) {
        selected_ui = u;
        u->setSelected(1);
    } else {
        u->setSelected(0);
    }
    uis.push_back(u);
}
void TW::board::selectNext()
{
    if (selected_ui == NULL || uis.size() == 1)
        return;
    selected_ui->setSelected(0);

    for (int i = 0; i < uis.size(); i++) {
        if (uis[i] == selected_ui) {
            if (i == uis.size() - 1) {
                selected_ui = uis.front();
                selected_ui->setSelected(1);
            } else {
                selected_ui = uis[i + 1];
                uis[i + 1]->setSelected(1);
            }
            break;
        }
    }
}
void TW::board::refresh()
{

    for (int i = 0; i < uis.size(); i++) {
        uis[i]->draw();
        wrefresh(uis[i]->win);
    }
}