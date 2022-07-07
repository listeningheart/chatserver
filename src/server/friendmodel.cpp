#include "friendmodel.hpp"
#include "db.h"

//添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)",userid, friendid);

    MySQL* mysql;
    if(mysql->connect())
    {
        mysql->update(sql);
    }
}



//返回用户好友列表，返回friend，并返回user中的对应信息
vector<User> FriendModel::query(int userid)
{
    //组装sql语句
    char sql[1023] = {0};
    //两个表的联合查询
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d",userid);
    
    MySQL mysql;
    vector<User> vec;

    if(mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr)
        {
            //把userid用户的所有离线消息放入vec返回
            MYSQL_ROW row;
            //mysql_fetch_row每次从MYSQL_RES*中获取一行。
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            //释放空间，防止内存泄漏
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}