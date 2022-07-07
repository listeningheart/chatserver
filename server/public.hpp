#ifndef PUBLIC_H
#define PUBLIC_H

enum EnMsgType{
    LOGIN_MSG=1,//登录
    LOGIN_MSG_ACK,//登录回应

    REG_MSG, //注册
    REG_MSG_ACK, //注册回应

    ONECHAT_MSG,//一对一聊天

    ADDFRIEND_MSG//添加好友业务
};

#endif