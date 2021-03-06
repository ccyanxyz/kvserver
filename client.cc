#include "node.h"
#include "utils.h"
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <unistd.h>

#define MAX_LINE 4096

using std::cout;
using std::cerr;
using std::endl;


static int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
static void print_help();
static void handle_interrupt(int signum);

int main(int argc, char *argv[])
{
	if(argc < 3) {
		cout << "Usage: ./client <ip> <port>" << endl;
		exit(0);
	}

	// register SIGINT
	signal(SIGINT, handle_interrupt);

	// int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		cerr << "create socket error:" << strerror(errno) << ", errno:" << errno << endl;
		exit(0);
	}
	
	// struct sockaddr_in {
	// 		sa_family_t sin_family;
	// 		in_port_t sin_port;
	// 		struct in_addr sin_addr;
	// }
	//
	// struct in_addr {
	// 		uint32_t s_addr;
	// }
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	// to big-endian
	server_addr.sin_port = htons(atoi(argv[2]));
	// convert x.x.x.x to int
	int flag = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	if(flag <= 0) {
		cerr << "get address error:" << strerror(errno) << ", errno:" << errno << endl;
		exit(0);
	}

	flag = connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(flag < 0) {
		cerr << "connect error:" << strerror(errno) << ", errno:" << errno << endl;
	}
	
	char buff[MAX_LINE];
	print_help();
	while(true) {
		cout << "~> ";
		memset(buff, 0, MAX_LINE);
		fgets(buff, MAX_LINE, stdin);
		buff[strlen(buff) - 1] = '\0';
		flag = send(socket_fd, buff, strlen(buff), 0);
		if(flag < 0) {
			cerr << "send error:" << strerror(errno) << ", errno:" << errno << endl;
			exit(0);
		}

		if(strcmp(buff, "exit") == 0) {
			cout << "exit" << endl;
			break;
		} else if(strcmp(buff, "help") == 0) {
			print_help();
			continue;
		}

		memset(buff, 0, MAX_LINE);
		int len = recv(socket_fd, buff, MAX_LINE, 0);
		if(len < 0) {
			cerr << "recv error:" << strerror(errno) << ", erron:" << errno << endl;
			exit(0);
		}
		buff[len] = '\0';

		Node *node = str_to_node(buff);
		if(node == NULL) {
			cout << buff << endl;
		} else if(node->key == "get") {
			if(node->value == "FAILED") {
				cout << "key not exist on server" << endl;
			} else {
				cout << "value:" << node->value << endl;
			}
		} else if(node->key == "put") {
			if(node->value == "SUCCESS") {
				cout << "put success" << endl;
			} else {
				cout << "put failed" << endl;
			}
		} else {
			cout << buff << endl;
		}
	}

	close(socket_fd);
	return 0;
}

void print_help()
{
	cout << "put <key>:<value>  store a key-value pair" << endl;
	cout << "get <key>  get the <value> of <key>" << endl;
	cout << "exit  exit client" << endl;
}

void handle_interrupt(int signum)
{
	std::string str("exit");
	send(socket_fd, str.c_str(), str.length(), 0);
	exit(0);
}
