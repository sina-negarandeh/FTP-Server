#include "Server.hpp"

using namespace std;

int server_socket_fd, client_socket_fd;
struct sockaddr_in server_address;
int opt = 1;

uint addrlen = sizeof(server_address);
int socket_fd;
int max_socket_fd;
int client_socket[CLIENTS_NUMBER];
fd_set read_fds;

int recv_message_size;

char message_buffer[MESSAGE_BUFFER_SIZE] = {0};

void ExitWithError(string error) {
	perror(error.c_str());
	exit(0);
}

void openCommandChannel() {
	for(int i = 0; i < CLIENTS_NUMBER; i++) {
		client_socket[i] = 0;
	}

	// Build address data structure
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT);

	// socket()
	if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ExitWithError("Failed to create a socket\n");
	}

	// Set socket options
	if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		ExitWithError("Failed to set sock options.\n");
	}

	// bind()
	if (bind(server_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		ExitWithError("Failed to bind a socket\n");
	}

	// Log
	printf("Server is listening on port %d\n", PORT);

	// listen()
	if (listen(server_socket_fd, 3) < 0) {
		ExitWithError("Failed to listen\n");
	}
}

void handleConnections() {
  // Log
  printf("Waiting for connections ...\n");

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(server_socket_fd, &read_fds);
		max_socket_fd = server_socket_fd;

		for(int client_index = 0; client_index < CLIENTS_NUMBER; client_index++) {
			socket_fd = client_socket[client_index];
			if(socket_fd > 0) {
				FD_SET(socket_fd, &read_fds);
			}
			if(socket_fd > max_socket_fd) {
				max_socket_fd = socket_fd;
			}
		}

		int activity = select(max_socket_fd + 1, &read_fds, NULL, NULL, NULL);
		if((activity < 0) && (errno != EINTR)) {
			ExitWithError("Failed to select\n");
		}

		if(FD_ISSET(server_socket_fd, &read_fds)) {

			if((client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&server_address, (socklen_t*)&addrlen)) < 0) {
				ExitWithError("Failed to accept\n");
			}

			// Log
			printf("Client connected to the server.\t\t");
			printf("Client IP:\t%s\tClient Port:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
			const char *send_buffer = "Connected to the server\n";
			send(client_socket_fd, send_buffer, strlen(send_buffer), 0);

			for (int client_index = 0; client_index < CLIENTS_NUMBER; client_index++) {
				if(client_socket[client_index] == 0) {
					client_socket[client_index] = client_socket_fd;
					break;
				}
			}
		}

		int client_index;
		for (client_index = 0; client_index < CLIENTS_NUMBER; client_index++) {
			socket_fd = client_socket[client_index];
			if (FD_ISSET(socket_fd , &read_fds)) {
				int number, valid;

				// recv()
				bzero(message_buffer, MESSAGE_BUFFER_SIZE);
				recv_message_size = recv(socket_fd, message_buffer, MESSAGE_BUFFER_SIZE, 0);
				if (recv_message_size < 0) {
				ExitWithError("recv() failed");
				} else if (recv_message_size == 0) {
					// Log
					printf("Client disconnected from the server.\t");
					printf("Client IP:\t%s\tClient Port:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
					close(socket_fd);
					client_socket[client_index] = 0;
				} else {
				message_buffer[MESSAGE_BUFFER_SIZE - 1] = '\0';
				int send_meesage_len = strlen(message_buffer) + 1;

				if(send(socket_fd, message_buffer, send_meesage_len, 0) != send_meesage_len) {
					ExitWithError("send() sent a different number of bytes than expected.\n");
				}
				printf("The client's message is %s", message_buffer);

				break;
				}

			}
		}
	}
}

void closeCommandChannel() {
	// close()
	close(server_socket_fd);
}
