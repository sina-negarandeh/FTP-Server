#include "User.hpp"

using namespace std;

User::User(std::string _ip_address, int _socket_addr){
    User::ip_address = _ip_address;
    User::port = _socket_addr;
    User::state = BASE_STATE;
}

void User::complete_user(User_s u){
    state = LOGGED_IN_STATE;
    data_remaining = u.size;
    admin = u.admin;
    path = "./";
}