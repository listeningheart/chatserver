#ifndef USERMODEL_H
#define USERMODEL_H
#include "user.hpp"

class UserModel
{
private:
    
public:
    bool insert(User &user);

    User query(int id);

    bool updateState(User user);

    bool resetState();

};


#endif