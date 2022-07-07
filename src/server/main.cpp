#include "chatserver.hpp"
#include "chatservice.hpp"
#include<iostream>
#include <signal.h>
using namespace std;

//处理服务器ctrl+c结束以后，重置user的状态信息。
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

//服务器主程序
int main()
{
    //捕获ctrl+c的信号
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "chatserver");

    server.start();
    loop.loop();
    return 0;
}