#include "Server.hpp"

using namespace std;

int server_socket_fd, client_socket_fd;
struct sockaddr_in server_address;
int opt = 1;
int command_channel_port, data_channel_port;
std::vector<User_s> config_users;
std::vector<std::string> files;
std::vector<User> connected_users;
char default_path[1024];

uint addrlen = sizeof(server_address);
int socket_fd;
int max_socket_fd;
int client_socket[CLIENTS_NUMBER];
fd_set read_fds;
int server_data_socket_fd, client_data_socket_fd;
struct sockaddr_in server_data_address;
int data_addrlen = sizeof(server_data_address);
int opt1;
char log_txt[MESSAGE_BUFFER_SIZE];

char data_buffer[MESSAGE_BUFFER_SIZE] = {0};

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

char * getDirectory( char * buf, int count){
    int i;
    int rslt = readlink("/proc/self/exe", buf, count - 1);
    if (rslt < 0 || (rslt >= count - 1))
    {
        return NULL;
    }
    buf[rslt] = '\0';
    for (i = rslt; i >= 0; i--)
    {
        if (buf[i] == '/')
        {
            buf[i + 1] = '\0';
            break;
        }
    }
    return buf;
}

void openDataChannel() {

    // Build address data structure
    server_data_address.sin_family = AF_INET;
    server_data_address.sin_addr.s_addr = INADDR_ANY;
    server_data_address.sin_port = htons(data_channel_port);

    // socket()
    if ((server_data_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ExitWithError("Failed to create a socket\n");
    }

    // Set socket options
    if (setsockopt(server_data_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt1, sizeof(opt1)) < 0) {
        ExitWithError("Failed to set sock options.\n");
    }

    // bind()
    if (bind(server_data_socket_fd, (struct sockaddr *) &server_data_address, sizeof(server_data_address)) < 0) {
        ExitWithError("Failed to bind a socket\n");
    }

    // Log
	log("data channel is listening on port " +  to_string(data_channel_port), "NOTIFICATION");
    // listen()
    if (listen(server_data_socket_fd, 3) < 0) {
        ExitWithError("Failed to listen\n");
    }
}

void accept_data_connection(){
	if ((client_data_socket_fd = accept(server_data_socket_fd, (struct sockaddr *)&server_data_address, (socklen_t *)&data_addrlen)) < 0) {
        ExitWithError("Failed to accept.\n");
    }
	printf("data connected\n");
}


int sendFile(FILE* file_ptr) {
    for (int i = 0; i < MESSAGE_BUFFER_SIZE; i++) {
        data_buffer[i] = fgetc(file_ptr);
        if (data_buffer[i] == EOF) {
            return 1;
        }
    }
    return 0;
}

void send_pure_data(string data){
	sprintf(data_buffer, "%s", data.c_str());
	send(client_data_socket_fd, data_buffer, strlen(data_buffer), 0);
}

int sendData(string file_name) {
    FILE* file_ptr = fopen(file_name.c_str(), "r");
    if (file_ptr == NULL) {
        ExitWithError("File open failed!\n");
    }
	int size = 0;
    while (1) {
        if (sendFile(file_ptr) == 1) {
            send(client_data_socket_fd, data_buffer, strlen(data_buffer), 0);
			size += strlen(data_buffer);
            break;
        } else {
            // send
            send(client_data_socket_fd, data_buffer, strlen(data_buffer), 0);
			size += strlen(data_buffer);
            bzero(data_buffer, MESSAGE_BUFFER_SIZE);
        }
    }

    fclose(file_ptr);
	return size;
}

void closeDataChannel() {
    // close()
    close(server_data_socket_fd);
}

int authenthicate_user(User &user){
	for (int i = 0 ; i < config_users.size() ; i++){
		if (config_users[i].user == user.username && config_users[i].password == user.password){
			user.complete_user(config_users[i]);
			return 0;
		}
	}
	user.state = BASE_STATE;
	return -1;
}

int find_user(int socket_fd){
	for (int i = 0 ; i < connected_users.size() ; i++){
		if (connected_users[i].socket_fd == socket_fd){
			return i;
		}
	}
	return -1;
}

bool find_file(string filename){
	for (int i = 0 ; i < files.size() ; i++){
		if (files[i] == filename){
			return true;
		}
	}
	return false;
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
	log("Server is listening on port " +  to_string(command_channel_port), "NOTIFICATION");

	// listen()
	if (listen(server_socket_fd, 3) < 0) {
		ExitWithError("Failed to listen\n");
	}

	openDataChannel();
}

std::string runCommand(std::string command, string options="", string argumants="") {
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
		if (options == ""){
			execl(("/bin/" + command).c_str(), command.c_str(), (char*)0);
		}else{
			execl(("/bin/" + command).c_str(), command.c_str(), options.c_str(), argumants.c_str(), (char*)0);
		}
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

string handle_cwd(vector<string> input, User &user){
	if (input.size() == 1){
		user.path = "./";
	}else{
		user.path += input[1];
	}
	if (user.path[user.path.length() - 1] != '/'){
		user.path += '/';
	}
	return "250: Successful change.\n";
}

std::string handleCommand(char *command, User &user) {
	string _command(command);
	vector<string> splitted_command = split_command(_command);

	if (splitted_command.size() == 0) return "Command not found\n";

	if (strcmp(splitted_command[0].c_str(), "quit") == 0) {

		return "221: Successful Quit.\n";

	}else if (strcmp(splitted_command[0].c_str(), "user") == 0){

		if (user.state != BASE_STATE) return "503: Bad sequence of commands.\n";
		if (splitted_command.size() < 2) return "501: Syntax error in parameters or argummants\n";
		user.set_username(splitted_command[1]);
		user.state = USER_STATE;
		return "331: User name okay, need password.\n";

	} else if (strcmp(splitted_command[0].c_str(), "pass") == 0){

		if (user.state != USER_STATE) return "503: Bad sequence of commands.\n";
		if (splitted_command.size() < 2) return "501: Syntax error in parameters or argummants\n";
		user.set_password(splitted_command[1]);
		if (authenthicate_user(user) < 0) return "430: Invalid username or password\n";
		return "230: User logged in, proceed. Logged out if appropriate.\n";

	} else if (strcmp(splitted_command[0].c_str(), "help") == 0) {

		std::ifstream ifs("help.txt");
		std::string content( (std::istreambuf_iterator<char>(ifs)),
							(std::istreambuf_iterator<char>()));
		ifs.close();
		cout<<content<<endl;
		return content;

	} else if (strcmp(splitted_command[0].c_str(), "pwd") == 0) {

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		return "257: " + user.path + "\n";  
		
	} else if (strcmp(splitted_command[0].c_str(), "mkd") == 0) { // mkd <directory path >

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		if (splitted_command.size() < 2) return "501: Syntax error in parameters or argummants\n";
		printf("command: mkd\n");
		runCommand("mkdir", "-p", user.path + splitted_command[1]);
		return "257: " + splitted_command[1] + " created" + "\n";

	} else if (strcmp(splitted_command[0].c_str(), "dele") == 0) {

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		if (splitted_command.size() < 3) return "501: Syntax error in parameters or argummants\n";
		
		if (strcmp(splitted_command[1].c_str(), "-f") == 0) {
			runCommand("rm", "-f", user.path + splitted_command[2]);
		} else if (strcmp(splitted_command[1].c_str(), "-d") == 0) {
			runCommand("rm", "-r", user.path + splitted_command[2]);
		}
		return "250: " + splitted_command[2] + " deleted.\n";

	} else if (strcmp(splitted_command[0].c_str(), "ls") == 0) {

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		string response = runCommand("ls", "-a", user.path);
		response += '\0';
		send_pure_data(response);
		return "226: List transfer done.\n";

	} else if (strcmp(splitted_command[0].c_str(), "cwd") == 0) {

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		if (splitted_command.size() < 2) return "501: Syntax error in parameters or argummants\n";
		return handle_cwd(splitted_command, user);

	} else if (strcmp(splitted_command[0].c_str(), "rename") == 0) {

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		if (splitted_command.size() < 3) return "501: Syntax error in parameters or argummants\n";
		if (rename((user.path + splitted_command[1]).c_str(), (user.path + splitted_command[2]).c_str()) == 0) {
			return "250: Successful change.\n";
		} else {
			return "500: Error.\n";
		}

	} else if (strcmp(splitted_command[0].c_str(), "retr") == 0) {

		if (user.state != LOGGED_IN_STATE) return "332 Need account for login!\n";
		if (splitted_command.size() < 2) return "501: Syntax error in parameters or argummants\n";
		if (find_file(splitted_command[1]) && (!user.admin)) return "550: File unavailable.\n";
		if (user.data_remaining <= 0){
			return "500: Error.\n";
		}
		int size = sendData(user.path + splitted_command[1]);
		user.data_remaining -= size;
		return "226: Successful Download\n";
	}
	return "Command not found\n";
}

void handleConnections() {
  // Log

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

			accept_data_connection();

			// Log
			bzero(log_txt, MESSAGE_BUFFER_SIZE);
			sprintf(log_txt, "New Client connected. Client IP: %s  -  Client Port: %d", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
			log(log_txt, "NOTIFICATION");
			connected_users.push_back(User(inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port), client_socket_fd));
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
					bzero(log_txt, MESSAGE_BUFFER_SIZE);
					sprintf(log_txt,"Client disconnected from the server. Client IP: %s  -  Client Port: %d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
					log(log_txt, "NOTIFICATION");
					close(socket_fd);
					client_socket[client_index] = 0;
				} else {

					int sender_index = find_user(socket_fd);
					bzero(log_txt, MESSAGE_BUFFER_SIZE);
					sprintf(log_txt,"%s:%d: %s", inet_ntoa(server_address.sin_addr), connected_users[sender_index].port, message_buffer);
					log(log_txt, "NOTIFICATION");
					if (sender_index < 0) ExitWithError("user was not found!"); 
					sprintf(message_buffer, "%s", handleCommand(message_buffer, connected_users[sender_index]).c_str());
					message_buffer[MESSAGE_BUFFER_SIZE - 1] = '\0';
					int send_meesage_len = strlen(message_buffer) + 1;
					bzero(log_txt, MESSAGE_BUFFER_SIZE);
					sprintf(log_txt,"%s", message_buffer);
					log(log_txt, "NOTIFICATION");
					if(send(socket_fd, message_buffer, send_meesage_len, 0) != send_meesage_len) {
						ExitWithError("send() sent a different number of bytes than expected.\n");
					}

					break;
				}

			}
		}
	}
	closeDataChannel();
}

void closeCommandChannel() {
	// close()
	close(server_socket_fd);
}