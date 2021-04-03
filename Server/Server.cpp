#include "Server.hpp"

using namespace std;

int server_socket_fd, client_socket_fd;
struct sockaddr_in server_address;
int opt = 1;
int command_channel_port, data_channel_port;
std::vector<User_s> config_users;
std::vector<std::string> files;
std::vector<User> connected_users;

uint addrlen = sizeof(server_address);
int socket_fd;
int max_socket_fd;
int client_socket[CLIENTS_NUMBER];
fd_set read_fds;

int recv_message_size;

char message_buffer[MESSAGE_BUFFER_SIZE] = {0};

vector<string> split_command(string _command, char delim=' '){
    vector<string> output;
    string part;
    stringstream s(_command);
    while (getline(s, part, delim)) {
		part = trim(part);
        output.push_back(part); 
    } 
    return output;
}

void openCommandChannel() {
	for(int i = 0; i < CLIENTS_NUMBER; i++) {
		client_socket[i] = 0;
	}

	// Build address data structure
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(command_channel_port);

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

std::string runCommand(std::string command, string options=NULL, string argumants=NULL) {
	int unnamed_pipe[2];
	pid_t pid;
	char result_buffer[MESSAGE_BUFFER_SIZE];

	if (pipe(unnamed_pipe) < 0) {
		ExitWithError("pipe() failed");
	}

	if ((pid = fork()) < 0) {
		ExitWithError("fork() failed");
	}

	if(pid == 0) {
		dup2 (unnamed_pipe[WRITE], STDOUT_FILENO);
		close(unnamed_pipe[READ]);
		close(unnamed_pipe[WRITE]);
		execl(("/bin/" + command).c_str(), command.c_str(), options.c_str(), argumants.c_str(), (char*)0);
		ExitWithError("execl() failed");
	} else {
		close(unnamed_pipe[WRITE]);
		int recv_message_size = read(unnamed_pipe[READ], result_buffer, sizeof(result_buffer));
		printf("Output: (%.*s)\n", recv_message_size, result_buffer);
		wait(NULL);
		return string(result_buffer);
	}
	return "error";
}

std::string handleCommand(char *command) {
	string _command(command);
	vector<string> splitted_command = split_command(_command);
	if (splitted_command.size() == 0) return "Command not found\n";
	if (strcmp(splitted_command[0].c_str(), "quit") == 0) {
		// TODO: check bug, Log Off user
		return "221: Successful Quit.\n";
	} else if (strcmp(splitted_command[0].c_str(), "help") == 0) {
		// TODO: Negarande Fix message
		return "214\n USER [name], Its argument is used to specify the user’s string. It is used for user authentication.\n";

	} else if (strcmp(splitted_command[0].c_str(), "pwd") == 0) {

		return "257: " + runCommand("pwd") + "\n";	//257: <working directory path>\n
		
	} else if (strcmp(splitted_command[0].c_str(), "mkd") == 0) { // mkd <directory path >
		//TODO: based on the user path!
		if (splitted_command.size() < 2) return "mkd needs one more argumant!";
		printf("command: mkd\n");
		runCommand("mkdir", "-p", splitted_command[1]);
		return "257: " + splitted_command[1] + " created" + "\n";

	} else if (strcmp(splitted_command[0].c_str(), "dele") == 0) {

		// TODO: Kamali Add runCommand
		printf("command: dele\n");

	} else if (strcmp(splitted_command[0].c_str(), "ls") == 0) {

		// TODO: Negarande Add runCommand
		printf("command: ls\n");
		// Send unCommand("ls") with data channel
		return "226: List transfer done.\n";

	} else if (strcmp(splitted_command[0].c_str(), "cwd") == 0) {

		// TODO: Kamali Add runCommand
		printf("command: cwd\n");

	} else if (strcmp(splitted_command[0].c_str(), "rename") == 0) {

		// TODO: Kamali Add runCommand
		printf("command: rename\n");
		return "250: Successful change.\n";

	} else if (strcmp(splitted_command[0].c_str(), "retr") == 0) {
		// TODO: Negarande Add runCommand
		printf("command: retr\n");
	}
	return "Command not found\n";
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
					printf("The client's message is %s", message_buffer);

					sprintf(message_buffer, "%s", handleCommand(message_buffer).c_str());
					message_buffer[MESSAGE_BUFFER_SIZE - 1] = '\0';
					int send_meesage_len = strlen(message_buffer) + 1;

					if(send(socket_fd, message_buffer, send_meesage_len, 0) != send_meesage_len) {
						ExitWithError("send() sent a different number of bytes than expected.\n");
					}

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
