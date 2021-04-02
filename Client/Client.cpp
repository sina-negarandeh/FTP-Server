#include "Client.hpp"

using namespace std;

int client_socket_fd;
struct sockaddr_in server_address;

//const char *message = "Hello from client!\n";
int recv_message_size;
char message_buffer[MESSAGE_BUFFER_SIZE] = {0};


void ExitWithError(string error) {
	perror(error.c_str());
	exit(EXIT_FAILURE);
}

void openCommandChannel() {

	// Build address data structure
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);

	// socket()
	if ((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ExitWithError("Failed to create a socket\n");
	}

	//
	if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
		ExitWithError("The address is invalid or unsupported.\n");
	}
}

void connectToServer() {
	// connect()
	if (connect(client_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		ExitWithError("Failed to establish a connection with the server.\n");
	}
}

void comiunicateWithServer() {
	// Authentication
	int is_user_logged_in = 1;

	// Sending Commands
	while (is_user_logged_in) {
		// recv()
		bzero(message_buffer, MESSAGE_BUFFER_SIZE);
		if ((recv_message_size = recv(client_socket_fd, message_buffer, MESSAGE_BUFFER_SIZE, 0)) < 0) {
			ExitWithError("recv() failed");
		}

		printf("The server's message is: %s", message_buffer);

		// send()
		bzero(message_buffer, MESSAGE_BUFFER_SIZE);
		printf("Enter your message: ");
		if (fgets(message_buffer, sizeof(message_buffer), stdin)) {
			message_buffer[MESSAGE_BUFFER_SIZE - 1] = '\0';
			int send_meesage_len = strlen(message_buffer) + 1;

			if (send(client_socket_fd, message_buffer, send_meesage_len, 0) != send_meesage_len) {
				ExitWithError("send() sent a different number of bytes than expected.\n");
			}

			if (strcmp("quit\n", message_buffer) == 0) {
				is_user_logged_in = 0;
				// Check if successful log out

				// recv()
				bzero(message_buffer, MESSAGE_BUFFER_SIZE);
				if ((recv_message_size = recv(client_socket_fd, message_buffer, MESSAGE_BUFFER_SIZE, 0)) < 0) {
					ExitWithError("recv() failed");
				}
				printf("%s", message_buffer);
			}
		}
	}
}

void closeCommandChannel() {
	// close()
	close(client_socket_fd);
}
