#include "../include/tw_socket.h"
#include <iostream>
#include <vector>
using namespace std;
TW::client* c;
vector<string> back_string;
int main(int argc, char* argv[])
{
    c = new TW::client(argv[1], 10004);
    while (1) {
        string input;
        cin >> input;
        c->sendMsg(input);
    }
    return 0;
}