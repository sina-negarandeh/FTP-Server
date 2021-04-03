#ifndef _JSONP_HPP
#define _JSONP_HPP

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
} User_s;

int parse(std::string path, int &commandChannelPort, int &dataChannelPort, std::vector<User_s> &users, std::vector<std::string> &files);

#endif