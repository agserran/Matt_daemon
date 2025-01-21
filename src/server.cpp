#include "server.hpp"

Server::Server()
{
	port = 4242;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	//log de abierto
}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server& Server::operator=(const Server &copy)
{
	*this = copy;
	return *this;
}

Server::~Server()
{
	close(socket_fd);
	//log de cerrado
}

void	Server::runServer()
{
	int	clientFd;
	int	epoll_fd = epoll_create(10);
	if (epoll_fd == -1)
	{
		//log de fallo de epoll
	}
}