#include "node.h"
#include "utils.h"
#include "cache.h"
#include "thread_pool.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <csignal>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE 4096

using std::cout;
using std::cerr;
using std::endl;

static std::mutex put_mutex, get_mutex;
static Cache cache;

static void handle_connection(int socket_fd);
static void handle_interrupt(int signum);

int main(int argc, char *argv[])
{
	if(argc < 2) {
		cerr << "Usage: ./server <port>" << endl;
		exit(0);
	}

	// register signal SIGINT and interrupt handler
	signal(SIGINT, handle_interrupt);

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1) {
		cerr << "create socket error:" << strerror(errno) << ", errno:" << errno << endl;
		exit(0);
	}

	struct sockaddr_in server_addr, client_addr;
	int sin_addr_len = sizeof(server_addr);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	// bind
	int flag = bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(flag == -1) {
		cerr << "bind socket error:" << strerror(errno) << ", errno:" << errno << endl;
		exit(0);
	}

	flag = listen(socket_fd, 10);
	if(flag == -1) {
		cerr << "listen socket error:" << strerror(errno) << ", errno:" << errno << endl;
		exit(0);
	}

	ThreadPool tp(3);

	cout << "waiting for connection..." << endl;
	while(true) {
		int connected_fd = accept(socket_fd, (struct sockaddr *) &client_addr, (socklen_t *) &sin_addr_len);
		if(connected_fd == -1) {
			cerr << "accept socket error:" << strerror(errno) << ", errno:" << errno << endl;
			continue;
		}
		tp.commit(handle_connection, connected_fd);
		cout << "client " << connected_fd - 3 << " connected from:" << inet_ntoa(client_addr.sin_addr) << ":" << htons(client_addr.sin_port) << ", idle thread number:" << tp.get_idle_num() << endl;
	}

	close(socket_fd);
	return 0;
}

void handle_interrupt(int signum)
{
	cache.save_cache();
	cout << "interrupt, server exited safely" << endl;
	exit(0);
}

void handle_connection(int socket_fd)
{
	char buff[MAX_LINE];
	memset(buff, 0, MAX_LINE);

	while(true) {
		int len = recv(socket_fd, buff, MAX_LINE, 0);
		if(len == 0) {
			continue;
		}
		std::vector<std::string> vec = split(buff, " ;:");
		if(vec[0] == "exit") {
			break;
		}
		if(vec[0] == "save") {
			cache.save_cache();
			static const std::string info = "all cache saved";
			if(send(socket_fd, info.c_str(), MAX_LINE, 0) == -1) {
				cerr << "send error" << endl;
			}
			cout << info << endl;
			continue;
		}
		memset(buff, 0, MAX_LINE);
		if(vec.size() == 2 && vec[0] == "get") {
			put_mutex.lock();
			std::string value = cache.get(vec[1]);
			put_mutex.unlock();
			if(send(socket_fd, ("get-" + value).c_str(), MAX_LINE, 0) == -1) {
				cerr << "send error" << endl;
			}
			if(!value.empty()) {
				cout << "search success, " << vec[1] << "-" << value << endl;
			} else {
				cout << "search failed, no key:" << vec[1] << endl;
			}
		} else if(vec.size() == 3 && vec[0] == "put") {
			put_mutex.lock();
			get_mutex.lock();
			cache.put(vec[1], vec[2]);
			get_mutex.unlock();
			put_mutex.unlock();
			if(send(socket_fd, "put-SUCCESS", MAX_LINE, 0) == -1) {
				cerr << "send error" << endl;
			}
			cout << "put success" << endl;
		} else {
			static const std::string info = "command error, try again";
			if(send(socket_fd, info.c_str(), MAX_LINE, 0) == -1) {
				cerr << "send error" << endl;
			}
			cout << info << endl;
		}
	}
	cout << "client " << socket_fd - 3 << " exited" << endl;
	close(socket_fd);
}
