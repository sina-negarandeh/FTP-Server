#include "User.hpp"

using namespace std;

User::User(std::string _ip_address, int _socket_addr){
    User::ip_address = _ip_address;
    User::socket_addr = _socket_addr;
    User::state = BASE_STATE;
}