cmake_minimum_required(VERSION 2.8.3)
project(chatroom)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread " )


#add_compile_options(-std=c++11)

add_library(tw_socket src/tw_socket.cpp)
add_library(tw_ui src/tw_ui.cpp)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)

add_executable(test_ui src/test_ui.cpp)

target_link_libraries(server tw_socket)
target_link_libraries(client tw_socket)

target_link_libraries(test_ui tw_ui curses)
