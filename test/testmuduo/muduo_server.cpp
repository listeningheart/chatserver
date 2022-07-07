

#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include<iostream>
#include<string>
#include<map>
#include"json.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;
using json = nlohmann::json;

class ChatServer
{
public:
    ChatServer(EventLoop *loop,       //事件循环
     const InetAddress &listenAddr,   //IP+port
     const string &nameArg)           //服务器名字
     :_server(loop, listenAddr, nameArg), _loop(loop)
    {
        //给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));//需要一个占位符，且由于调用的是成员函数需要有this指针。

        //给服务器已注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        //设置服务器端线程的数量，4则为1个I/O线程，3个worker线程
        _server.setThreadNum(4);

    }

    //启动服务
    void start(){
        _server.start();
    };

private:
    //两个回调函数，新连接事件与读写事件，实现对事件的处理
    void onConnection(const TcpConnectionPtr &conn)
    {
        if(conn->connected())
        {
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<" successed"<<endl;
        }
        else
        {
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<" failed"<<endl;
            conn->shutdown();//fclose(fd);
        }
    }

    //上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &conn,//连接
     Buffer *buffer,//缓冲区
      Timestamp time)//接受的数据的时间
      {
        string buf = buffer->retrieveAllAsString();
        cout<<"recv data:"<<buf<<" time:"<<time.toString()<<endl;
        conn->send(buf);//回传
    }

    TcpServer _server;  //组合的muduo，实现服务器功能的类对象
    EventLoop *_loop;   //epoll
};

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "CharServer");

    server.start(); //listenfd epoll_ctl->epoll
    loop.loop(); //epoll_wait()以阻塞方式等待新用户链接事件，已连接用户的读写事件。

}