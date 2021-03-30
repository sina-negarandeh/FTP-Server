#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

typedef struct Users{
    std::string user;
    std::string password;
    bool admin;
    int size;
} User;

int parse(std::string path, int &commandChannelPort, int &dataChannelPort, std::vector<User> &users, std::vector<std::string> &files);