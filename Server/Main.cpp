#include "Server.hpp"

int main (int argc, char const *argv[]) {

	parse(CONFIG_PATH, command_channel_port, data_channel_port, config_users, files);
	openCommandChannel();
	handleConnections();
	closeCommandChannel();

	return 0;
}
