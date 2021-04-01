#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 8080
#define MESSAGE_BUFFER_SIZE 1024

void ExitWithError(std::string error);

void openCommandChannel();

void connectToServer();

void comiunicateWithServer();

void closeCommandChannel();

#endif
