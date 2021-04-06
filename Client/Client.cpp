#include "Client.hpp"

using namespace std;

int client_socket_fd;
int command_channel_port, data_channel_port;
struct sockaddr_in server_address;
//data channel
int client_data_socket_fd;
struct sockaddr_in server_data_address;
char data_buffer[MESSAGE_BUFFER_SIZE] = {0};
int recv_data_size;
int download = 0;
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
	server_address.sin_port = htons(command_channel_port);

	// socket()
	if ((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ExitWithError("Failed to create a socket\n");
	}

	//
	if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
		ExitWithError("The address is invalid or unsupported.\n");
	}
	cout<<"command channel oppened!"<<endl;
}

void connectToServer() {
	// connect()
	if (connect(client_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		ExitWithError("Failed to establish a connection with the server.\n");
	}
	cout<<"connected to server!"<<endl;
}

void openDataChannel() {
    // Build address data structure
    server_data_address.sin_family = AF_INET;
    server_data_address.sin_port = htons(data_channel_port);

    // socket()
    if ((client_data_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ExitWithError("Failed to create a socket\n");
    }

    //
    if (inet_pton(AF_INET, "127.0.0.1", &server_data_address.sin_addr) <= 0) {
        ExitWithError("The address is invalid or unsupported.\n");
    }
}

void connectToDataServer() {
    // connect()
    if (connect(client_data_socket_fd, (struct sockaddr *) &server_data_address, sizeof(server_data_address)) < 0) {
        ExitWithError("Failed to establish a connection with the server.\n");
    }
}

int recvFile()
{
    char ch;
    for (int i = 0; i < MESSAGE_BUFFER_SIZE; i++) {
        ch = data_buffer[i];
        if (ch == EOF)
            return 1;
        else
            printf("%c", ch);
    }
    return 0;
}

void recvData() {
	ofstream MyFile("download" + to_string(download) + ".txt");
	download+=1;
	bzero(data_buffer, MESSAGE_BUFFER_SIZE);
	if ((recv_data_size = recv(client_data_socket_fd, data_buffer, MESSAGE_BUFFER_SIZE, 0)) < 0) {
		ExitWithError("recv() failed");
	}
	MyFile<<data_buffer;
	MyFile.close();
}

void recv_pure_data(){
	bzero(data_buffer, MESSAGE_BUFFER_SIZE);
	if ((recv_data_size = recv(client_data_socket_fd, data_buffer, MESSAGE_BUFFER_SIZE, 0)) < 0) {
		ExitWithError("recv() failed");
	}
	cout<<"data:"<<endl<<data_buffer<<endl;
}

void closeDataChannel() {
    // close()
    close(client_data_socket_fd);
}

void update_logged_in(int &is_user_logged_in, string message){
	if (message[0] == '2' && message[1] == '3' && message[2] == '0')
		is_user_logged_in = 1;
}

void comiunicateWithServer() {
	// Authentication
	int is_user_connected = 1;
	int is_user_logged_in = 0;
	usleep(200);
	openDataChannel();
	connectToDataServer();
	// Sending Commands
	while (is_user_connected) {
		// recv()
		bzero(message_buffer, MESSAGE_BUFFER_SIZE);
		if ((recv_message_size = recv(client_socket_fd, message_buffer, MESSAGE_BUFFER_SIZE, 0)) < 0) {
			ExitWithError("recv() failed");
		}

		printf("%s", message_buffer);
		update_logged_in(is_user_logged_in, message_buffer);

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
				is_user_connected = 0;
				// Check if successful log out

				// recv()
				bzero(message_buffer, MESSAGE_BUFFER_SIZE);
				if ((recv_message_size = recv(client_socket_fd, message_buffer, MESSAGE_BUFFER_SIZE, 0)) < 0) {
					ExitWithError("recv() failed");
				}
				printf("%s", message_buffer);
			}else if (strcmp("ls\n", message_buffer) == 0 && is_user_logged_in){
				recv_pure_data();
			}else if (strncmp("retr", message_buffer, 4) == 0 && is_user_logged_in){
				recvData();
			}
		}
	}
	closeDataChannel();
}

void closeCommandChannel() {
	// close()
	close(client_socket_fd);
}