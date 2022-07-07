#include "usermodel.hpp"
#include "db.h"

bool UserModel::insert(User &user)
{
    //组装sql语句
    char sql[1023] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s','%s','%s')",
    user.getName().c_str(),user.getPwd().c_str(),user.getState().c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            //获取插入成功的用户数据生成的主键id.
            //mysql_insert_id() 返回最后一个query中自动生成的 ID
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    //查询id是否存在，若存在返回User类。
    //组装sql语句
    char sql[1023] = {0};
    User user;
    sprintf(sql, "select * from user where id = %d" , id);

    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr)
        {
            //查询成功,对应数据存入user类,row得到的是char *
            MYSQL_ROW row = mysql_fetch_row(res);
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]);
            //*****************释放res，防止内存泄漏
            mysql_free_result(res);
        }
    }
    //若成功则正确值，若失败则默认值。
    return user;
}

// 更新用户的状态信息
bool UserModel::updateState(User user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

bool UserModel::resetState()
{
    //将已有user的state数据设为'offline'-更新到数据库。
    //组装sql语句
    char sql[1023] = {0};
    sprintf(sql, "update user set state = 'offline'");
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            //成功
            return true;
        }
    }
    return false;
}