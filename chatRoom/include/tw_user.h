#ifndef TW_USERS_MANA_H_
#define TW_USERS_MANA_H_
#include <string>
namespace TW {
namespace user {
    struct account {
        std::string name;
        int socket_fd; //对应的socket
        uint thread_index; //对应的线程index
    };
}
}
#endif