#include "../include/tw_net.h"
#include <iostream>
#include <vector>
using namespace std;
TW::net::client* c;
vector<string> back_string;
int main(int argc, char* argv[])
{
    c = new TW::net::client(argv[1], 10004);
    while (1) {
        string input;
        cin >> input;
        c->sendMsg(input);
    }
    return 0;
}