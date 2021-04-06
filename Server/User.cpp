#include "User.hpp"

using namespace std;

User::User(std::string _ip_address, int _socket_addr, int _socket_fd){
    User::ip_address = _ip_address;
    User::port = _socket_addr;
    User::state = BASE_STATE;
    User::socket_fd = _socket_fd;
}

void User::complete_user(User_s u){
    state = LOGGED_IN_STATE;
    data_remaining = u.size;
    admin = u.admin;
    path = "./";
}