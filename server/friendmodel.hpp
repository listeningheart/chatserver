#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include<usermodel.hpp>
#include<vector>
#include<iostream>
using namespace std;
//维护好友信息的操作接口方法。//操作firend表的接口方法类
class FriendModel
{
    public:
    //添加好友关系
    void insert(int userid, int friendid);
    //删除好友关系
    // void remove(int userid, int friendid);
    //返回用户好友列表，返回friend，并返回user中的对应信息
    vector<User> query(int userid);
};


#endif
