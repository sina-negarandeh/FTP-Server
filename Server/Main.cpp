#include "Server.hpp"

int main (int argc, char const *argv[]) {

	openCommandChannel();
	handleConnections();
	closeCommandChannel();

	return 0;
}
