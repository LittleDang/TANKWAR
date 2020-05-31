#ifndef TW_UI_H_
#define TW_UI_H_
#include <curses.h>
#include <list>
#include <mutex>
#include <string>
#include <vector>

/*
1. 每一个ui都是继承了box_ui，为了调用的方便，我把成员都设置为public，不是个好习惯，但是爽。。
2. 每个ui都注册到board实例里面，由board实例统一管理
3. board有一个独立线程用来读取键值，并且根据选中与否，
将键值信息发送到对应ui的缓冲区（这样子可以统一管理ui的键盘事件以及让多个ui存在于同一个线程，在等待输入的时候不会阻塞程序）
4. 每个ui根据自己的缓冲区按照自己的draw函数去处理
*/
namespace TW {
namespace ui {
    class box_ui { //基类，所有的ui都要继承这个基类
    public:
        box_ui(int x_, int y_, int w_, int h_);
        ~box_ui();
        void setPos(int x_, int y_); //本质上是通过删除原来的win，重新创建一个win
        void setSize(int w_, int h_); //本质上是通过删除原来的win，重新创建一个win
        void setSelected(int f); //表示选中与否，选中状态下，board会把键盘信号发送到该ui的缓存区

        virtual void draw() = 0; //纯虚函数，没有实现，待派生类实现，主要用来提供接口让board统一管理
        //主要用来可视化的操作
        virtual void logic() = 0; //纯虚函数，没有实现，待派生类实现，主要用来提供接口让board统一管理
        //主要用来处理一些逻辑上的东西，例如缓存区

        void setKey(char ch); //主要是提供给board写键盘信号到缓存区
        std::mutex key_lock; //用来保证缓冲区读和写不同时操作
        WINDOW* win;
        int selected;
        int x, y, w, h;
        std::list<char> buff;
    };

    //--------------------------------------------------------------------------
    class text_box : public box_ui { //文本框
    public:
        text_box(int x_, int y_, int w_, int h_);
        void add_text(std::string s);
        void clear_text(std::string s);
        void draw();
        void logic();

    private:
        std::vector<std::string> text;
    };
    //--------------------------------------------------------------------------

    class input_box : public box_ui { //输入框
    public:
        input_box(int x_, int y_, int w_, int h_);
        std::string getText();
        void draw();
        void logic();

    private:
        std::list<std::string> output_text;
        std::string showText;
    };
    //--------------------------------------------------------------------------
    class draw_box : public box_ui {
    public:
        draw_box(int x_, int y_, int w_, int h_);
        void draw();
        void logic();

        void setPixel(int x_, int y_, std::string ch);

    private:
        std::vector<std::string> pixel;
    };
    //--------------------------------------------------------------------------
    class board { //画板类，负责管理ui
    public:
        board();
        void addUi(box_ui* u);
        void selectNext();
        void refresh();
        void keyFuntor();
        void createKeythread(); //创建一条线程来监听键盘信号

    private:
        std::vector<box_ui*> uis;
        box_ui* selected_ui;
        int destroy;
    };
}
}
#endif