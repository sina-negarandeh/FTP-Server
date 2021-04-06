#include "Client.hpp"

int main (int argc, char const *argv[]) {

	std::vector<User_s> users;
	std::vector<std::string> files;
	parse(CONFIG_PATH, command_channel_port, data_channel_port, users, files);
	openCommandChannel();
	connectToServer();
	comiunicateWithServer();
	closeCommandChannel();

	return 0;
}
