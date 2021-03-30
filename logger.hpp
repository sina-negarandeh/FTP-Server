#ifndef _LOGGER_HPP
#define _LOGGER_HPP

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>    
#include <string>

#define FILENAME "log.txt"
#define WHITESPACE " \t\n\r\f\v"

std::string trim(std::string s);
int log(std::string message, std::string type);

#endif