#include "Client.hpp"

int main (int argc, char const *argv[]) {
	openCommandChannel();
	connectToServer();
	comiunicateWithServer();
	closeCommandChannel();

	return 0;
}
