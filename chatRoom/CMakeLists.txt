cmake_minimum_required(VERSION 2.8.3)
project(chatroom)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -pthread " )


#add_compile_options(-std=c++11)

add_library(tw_net src/tw_net.cpp)
add_library(tw_ui src/tw_ui.cpp)

add_executable(test_server src/test_server.cpp)
add_executable(test_client src/test_client.cpp)

add_executable(test_ui src/test_ui.cpp)

target_link_libraries(test_server tw_net tw_ui curses)
target_link_libraries(test_client tw_net tw_ui curses)

target_link_libraries(test_ui tw_ui curses)



