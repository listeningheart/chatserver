#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);


    //启动服务
    void start();
private:
    //创建链接和读写事件的回调函数
    void onConnection(const TcpConnectionPtr&);
    void onMessage(const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp);

    TcpServer _server;
    EventLoop *_loop;
};

#endif