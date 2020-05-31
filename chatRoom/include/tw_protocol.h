#ifndef TW_PROTOCOL_H_

#define TW_PROTOCOL_H_
//定义了一些协议
//目前采用的是比较笨又比较简单的协议方式，没有加密
//发送的数据报前两个char的内存为一个ushort，表示数据报的len（不包括这两个字符）
//即len(PROTO_XXX+data)+data
//数据报的类型根据PROTO_XXX分类
#include <string>
#define PROTO_LOGIN (std::string("login ")) //表示登陆
#define PROTO_SPEAK (std::string("speak ")) //表示发言
#define PROTO_ERROR (std::string("error ")) //表示错误
#endif