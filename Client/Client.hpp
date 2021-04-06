#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <sys/socket.h>
#include "json_parser.hpp"

#define CONFIG_PATH "./config.json"
#define MESSAGE_BUFFER_SIZE 1024

extern int command_channel_port, data_channel_port;

void ExitWithError(std::string error);

void openCommandChannel();

void connectToServer();

void openDataChannel();

void connectToServer();

void recvData();

int recvFile();

void closeDataChannel();

void connectToDataServer();

void comiunicateWithServer();

void closeCommandChannel();

#endif
