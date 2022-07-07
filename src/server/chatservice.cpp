#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h> //日志
#include <iostream>
using namespace std;

ChatService *ChatService::instance()
{
    static ChatService service; //局部静态变量，是线程安全的。
    return &service;
}

//注册消息id以及对应的Handler回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONECHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADDFRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
}
//服务器异常，业务充值方法
void ChatService::reset()
{
    _userModel.resetState();
}

//获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    //记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        cout << "error" << endl;
        LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        //返回一个空操作，默认的处理器****这个很细节，统一handler的返回格式。
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) { //=获取外部变量
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

//处理登录业务  ORM Object relation map 对象关系映射。 业务层操作的都是对象。 DA层操作数据库。
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //登录的时候输入id
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id); //查询id是否存在
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            //已登录
            json responce;
            responce["msgid"] = LOGIN_MSG_ACK;
            responce["errno"] = 2;
            responce["errmsg"] = "this account is used, try another input!";
            conn->send(responce.dump());
        }
        else
        {
            //未登录，密码正确。记录userconnmap，更新用户状态-上传数据库，回复消息给client
            _userConnMap.insert({id, conn});
            user.setState("online");
            _userModel.updateState(user);
            json responce;
            responce["msgid"] = LOGIN_MSG_ACK;
            responce["errno"] = 0;
            responce["id"] = user.getId();
            responce["name"] = user.getName();

            //查询该用户是否有离线消息
            vector<string> vec  = _offlineMsgModel.query(id);
            if(!vec.empty())
            {
                responce["offlinemsg"] = vec;
                //读取该用户的离线信息后，将数据库中存储的离线信息删除
                _offlineMsgModel.remove(id);
            }

            //查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if(!userVec.empty())
            {
                vector<string> vec2;
                for(User &user:userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                responce["friends"] = vec2;
            }

            conn->send(responce.dump());
        }
    }
    else
    {
        //密码错误，回复消息给client
        json responce;
        responce["msgid"] = LOGIN_MSG_ACK;
        responce["errno"] = 1;
        responce["errmsg"] = "you have the wrong password!";
        conn->send(responce.dump());
    }
}

//处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        //注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump()); // dump序列化，可使用json::parse()反序列化为json对象。
    }
    else
    {
        //注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

//一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();
    //一对一聊天业务有两种情况，对方在线或不在线,可通过查找_userConnMap
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userConnMap.find(toid);
        //若找到
        if(it != _userConnMap.end())
        {
            //toid在线，转发消息。
            it->second->send(js.dump());
            return ;
        }
    }

    //不在线,放入离线消息数据库。
    _offlineMsgModel.insert(toid,js.dump());        
}

//处理客户端意外退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    //处理数据库中state，删除userConnMap中元素
    User user;
    {///仅对_userConnMap的操作需要互斥，所以加上一个作用域。
        lock_guard<mutex> lock(_connmutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // map表中删除用户信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    //更新用户的状态信息,数据库自己保证信息互斥。
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::addFriend(const TcpConnectionPtr&, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    _friendModel.insert(userid, friendid);
}
