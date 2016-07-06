#include <string>


struct TIpmiNode
{
    string  id;
    string  address;
    string  port;
    string  login;
    string  password;


};

struct THandlerConfig
{
    string rackName = "1-1";
    bool debug = "false";
    vector<TIpmiNode> IpmiNodes;
    int timeout;
};
