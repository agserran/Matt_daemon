#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "tintin.hpp"

#define MAX_CLIENTS 3
#define MAX_EVENTS 10

class Tintin;

class Server
{
private:
	int port;
	struct sockaddr_in servAddr;
	int socket_fd;
	std::set<int> clients;
	Tintin *tintin = new Tintin();
	int lock_fd;

public:
	Server();
	Server(const Server &copy);
	Server &operator=(const Server &other);
	~Server();
	void runServer();
	void signalHandler();
	void shutDown(int socket_fd, int epoll_fd);
	void createFile();
	void unlinkFile();
};

#endif