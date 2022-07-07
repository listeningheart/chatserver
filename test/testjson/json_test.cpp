#include<iostream>
#include<string>
#include<map>
#include"json.hpp"
using namespace std;
using json = nlohmann::json;

int main()
{
    json js;
    js["id"] = {1,2,3};
    js["name"] = "adfa";
    js["msg"]["z x"] = "hello ch";
    js["msg"]["a b"] = "hello en";
    cout <<js<<endl;
    cout<<"----------------"<<endl;
    string jsstr = js.dump();
    cout<<jsstr<<endl;
    cout<<"----------------"<<endl;
    cout<<json::parse(jsstr)["id"]<<endl;
    cout<<"----------------"<<endl;
    map<string,int> m = {{"af",1},{"ii",3}};
    cout<<m["af"]<<endl;
    cout<<m["ii"]<<endl;
    return 0;
}
