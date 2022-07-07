#ifndef CHATSERVICE_H
#define CHATSERVICE_H


#include <muduo/net/TcpServer.h>
#include <unordered_map>
#include <functional>
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "json.hpp"
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;
//回调函数通用对象。
using MsgHandler = std::function<void(const TcpConnectionPtr&, json &js, Timestamp)>;



//聊天服务器业务类,使用单例模式
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService* instance();

    //处理登录业务,用到user数据操作类
    void login(const TcpConnectionPtr&, json &js, Timestamp);
    //处理注册业务
    void reg(const TcpConnectionPtr&, json &js, Timestamp);
    //一对一聊天业务,用到user数据操作类和offlinemsg数据操作类
    void oneChat(const TcpConnectionPtr&, json &js, Timestamp);
    //客户端退出。
    void clientCloseException(const TcpConnectionPtr&);
    //添加好友业务
    void addFriend(const TcpConnectionPtr&, json &js, Timestamp);

    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //服务器异常，重置业务
    void reset();
private:
    //私有化默认构造函数
    ChatService();
    //存储信息id和对应业务的回调处理函数
    unordered_map<int, MsgHandler> _msgHandlerMap;
    //存储已登录用户与链接的映射,用户上线记录（登录），下线删除（断开连接）
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    //_userConnMap互斥锁。
    mutex _connmutex;


    //数据处理类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
};

#endif