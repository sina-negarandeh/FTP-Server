#include <vector>
#include <string>

#define BASE_STATE 0
#define USER_STATE 1
#define LOGGED_IN_STATE 2


class User{
    public:
    User(std::string _ip_address, int _socket_addr);

    std::string ip_address;
    int socket_addr;
    std::string current_path;
    int data_remaining;
    int state;
    
    // private:
    // int idiot_counter = 0;
};
