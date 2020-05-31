#include "../include/tw_ui.h"
#include <thread>
#include <unistd.h>
TW::ui::box_ui::box_ui(int x_, int y_, int w_, int h_)
{
    x = x_;
    y = y_;
    h = h_;
    w = w_;
    win = newwin(h_, w_, y_, x_);
    selected = 0;
}
TW::ui::box_ui::~box_ui()
{
    delwin(win);
}

void TW::ui::box_ui::setPos(int x_, int y_)
{
    x = x_;
    y = y_;
    delwin(win);
    win = newwin(h, w, y, x);
}
void TW::ui::box_ui::setSize(int w_, int h_)
{
    w = w_;
    h = h_;
    delwin(win);
    win = newwin(h, w, y, x);
}

void TW::ui::box_ui::setSelected(int f)
{
    selected = f;
}
void TW::ui::box_ui::setKey(char ch)
{
    buff.push_back(ch);
    if (buff.size() > 1024) //缓存上限1024char 超过自己清除。
        buff.pop_front();
}

//--------------------box

TW::ui::text_box::text_box(int x_, int y_, int w_, int h_)
    : box_ui(x_, y_, w_, h_)
{
}

void TW::ui::text_box::add_text(std::string s)
{
    text.push_back(s);
}

void TW::ui::text_box::clear_text(std::string)
{
    text.clear();
}

void TW::ui::text_box::logic()
{
    //do nothing
}
void TW::ui::text_box::draw()
{
    wclear(win);
    for (int i = 0; i < h - 2; i++) {
        int y = text.size(); //to int
        y = y - i - 1;
        if (y < 0)
            break;
        for (int j = 0; j < text[y].size(); j++) {
            mvwaddch(win, h - i - 2, j + 1, text[y][j]);
        }
    }
    if (selected)
        box(win, '|', '-');
    else
        box(win, ' ', ' ');
    //wrefresh(win);
}

//-------------------------text box

TW::ui::input_box::input_box(int x_, int y_, int w_, int h_)
    : box_ui(x_, y_, w_, h_)
{
}
int isVaildCh(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return 1;
    if (ch >= 'A' && ch <= 'Z')
        return 1;
    if (ch >= '0' && ch <= '9')
        return 1;
    if (ch == ' ')
        return 1;
    return 0;
}
void TW::ui::input_box::logic()
{
    char ch;
    //读缓冲区，先锁定缓冲区
    key_lock.lock();
    while (!buff.empty()) {
        ch = buff.front();
        buff.pop_front();
        if (ch == '\n') //回车，表示输入结束
        {
            //showText.push_back('\0');
            output_text.push_back(showText);
            showText.clear();
        } else if (ch == 7) { //退格键
            if (!showText.empty())
                showText.pop_back();
        } else if (isVaildCh(ch)) {
            showText.push_back(ch);
        }
    }
    key_lock.unlock(); //解锁
}
void TW::ui::input_box::draw()
{
    if (!selected) {
        wclear(win);
        if (!showText.empty())
            mvwprintw(win, 1, 1, "%s", showText.c_str());
        box(win, ' ', ' ');
        //wrefresh(win);
    } else {
        wclear(win);
        if (!showText.empty()) {
            for (int i = 0; i < showText.length(); i++) {
                mvwaddch(win, 1, 1 + i, showText[i]);
            }
        }
        box(win, '|', '-');
    }
}

std::string TW::ui::input_box::getText()
{
    if (output_text.empty())
        return "";
    std::string s = output_text.front();
    output_text.pop_front();
    return s;
}
//----------------------inputbox
TW::ui::draw_box::draw_box(int x_, int y_, int w_, int h_)
    : box_ui(x_, y_, w_, h_)
{
    x = x_;
    y = y_;
    h = h_;
    w = w_;
    win = newwin(h_ + 2, w_ * 2 + 2, y_, x_);
    selected = 0;
    pixel.resize(w_ * h_);
}
void TW::ui::draw_box::logic()
{
    //do nothing
}
void TW::ui::draw_box::draw()
{
    wclear(win);
    for (int i = 0; i < pixel.size(); i++) {
        int r = i / w;
        int c = (i % w) * 2;
        if (pixel[i].size() >= 2) {
            mvwaddch(win, 1 + r, 1 + c, pixel[i][0]);
            mvwaddch(win, 1 + r, 2 + c, pixel[i][1]);
        }
    }
    if (selected)
        box(win, '|', '-');
    else
        box(win, ' ', ' ');
}
void TW::ui::draw_box::setPixel(int x_, int y_, std::string ch)
{
    if (x_ < 0 || x_ >= w || y_ < 0 || y_ >= h)
        return;
    pixel[x_ + y_ * w] = ch;
}

//----------------------drawbox

TW::ui::board::board()
{
    selected_ui = NULL;
}
void TW::ui::board::keyFuntor()
{
    while (1) {
        char ch = getch();
        if (ch != 9) {
            if (selected_ui) {
                selected_ui->key_lock.lock(); //写缓冲区，先锁定
                selected_ui->setKey(ch);
                selected_ui->key_lock.unlock();
            }
        } else {
            selectNext();
        }
        usleep(10000);
    }
}
void TW::ui::board::createKeythread()
{
    std::thread thr(std::bind(&TW::ui::board::keyFuntor, this));
    thr.detach();
}

void TW::ui::board::addUi(box_ui* u)
{
    if (selected_ui == NULL) {
        selected_ui = u;
        u->setSelected(1);
    } else {
        u->setSelected(0);
    }
    uis.push_back(u);
}
void TW::ui::board::selectNext()
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
void TW::ui::board::refresh()
{

    for (int i = 0; i < uis.size(); i++) {
        uis[i]->logic();
        uis[i]->draw();
        wrefresh(uis[i]->win);
    }
}