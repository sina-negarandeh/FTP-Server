#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "logger.hpp"
#include "User.hpp"
#include "json_parser.hpp"

#define PORT 8080
#define CLIENTS_NUMBER 30
#define MESSAGE_BUFFER_SIZE 1024
#define CONFIG_PATH "./config.json"

#define READ 0
#define WRITE 1

extern int command_channel_port, data_channel_port;
extern std::vector<User_s> config_users;
extern std::vector<std::string> files;
extern char default_path[1024];

void ExitWithError(std::string error);

void openCommandChannel();

char * getDirectory( char * buf, int count);

std::string handleCommand(char *command);

void handleConnections();

void closeCommandChannel();

void openDataChannel();

int sendData(std::string file_name);

void closeDataChannel();

int sendFile(FILE* file_ptr);

#endif
