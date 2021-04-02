#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define CLIENTS_NUMBER 30
#define MESSAGE_BUFFER_SIZE 1024

#define READ 0
#define WRITE 1

void ExitWithError(std::string error);

void openCommandChannel();

std::string runCommand(std::string command);

std::string handleCommand(char *command);

void handleConnections();

void closeCommandChannel();

#endif
