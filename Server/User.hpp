#include <vector>
#include <string>
#include "json_parser.hpp"

#define BASE_STATE 0
#define USER_STATE 1
#define LOGGED_IN_STATE 2


class User{
    public:
    User(std::string _ip_address, int _socket_addr, int _socket_fd);
    void set_username(std::string _username){username = _username;}
    void set_password(std::string _password){password = _password;}
    void complete_user(User_s u);
    std::string ip_address;
    int port;
    int socket_fd;
    std::string path;
    int data_remaining;
    int state;
    std::string username;
    std::string password;
    bool admin;
    
    // private:
    // int idiot_counter = 0;
};
