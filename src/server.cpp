#include "../inc/server.hpp"

Server::Server()
{
	port = 4242;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	tintin->writeLog("[INFO] creating server");
}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server& Server::operator=(const Server &other)
{
 	delete tintin;
  port      = other.port;
  servAddr  = other.servAddr;
  socket_fd = other.socket_fd;
  clients   = other.clients;
  tintin = new Tintin(*other.tintin);
	return *this;
}

Server::~Server()
{
	close(socket_fd);
	tintin->writeLog("[INFO] Server closed");
	delete tintin;
}

void Server::unlinkFile()
{
	if (unlink("/var/lock/matt_daemon.lock") < 0)
	{
		tintin->writeLog("[ERROR] unlink");
	}
}

void Server::sutDown(int socket_fd, int epoll_fd)
{
	for (int fd : Server::clients)
	{
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
		close(fd);
	}
	Server::clients.clear();
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, nullptr);
	close(socket_fd);
	close(epoll_fd);
	tintin->writeLog("[INFO] Server closed");
	delete(tintin);
	unlinkFile();
}

void	Server::runServer()
{
	int	epoll_fd = epoll_create(10);
	if (epoll_fd == -1)
	{
		tintin->writeLog("[ERROR] error epoll");
		exit(EXIT_FAILURE);
	}
	struct epoll_event event, events[MAX_EVENTS];
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = socket_fd;
	int flags  = fcntl(socket_fd, F_GETFL, 0);
	if (flags < 0)
	{
		tintin->writeLog("[ERROR] F_GETFL.");
		delete(tintin);
		exit(EXIT_FAILURE);
	}
	if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		tintin->writeLog("[ERROR] fcntl error");
		close(socket_fd);
		delete(tintin);
		exit(EXIT_FAILURE);
	}

	if (bind(socket_fd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
			tintin->writeLog(std::string("[ERROR] bind failed"));
			exit(EXIT_FAILURE);
	}

	// 3) listen()
	if (listen(socket_fd, MAX_CLIENTS) < 0) {
			tintin->writeLog(std::string("[ERROR] listen failed"));
			exit(EXIT_FAILURE);
	}

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
	{
		tintin->writeLog("[ERROR] error creating socker fd");
		close(socket_fd);
		close(epoll_fd);
		delete(tintin);
		exit(EXIT_FAILURE) ;
	}
	while (true)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

		if (num_events < 0)
		{
			if (num_events == EINTR)
				continue;
			tintin->writeLog("[ERROR] epoll_wait");
			break;
		}
		for (int i = 0; i < num_events; i++)
		{
			if (events[i].data.fd == socket_fd)
			{
				struct sockaddr_in clientAddr;
				socklen_t addrlen = sizeof(clientAddr);
				int clientFd = accept(socket_fd, (struct sockaddr*)&clientAddr, &addrlen);
				if (clientFd == -1)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break;
					else{
						tintin->writeLog("Error: accept failed");
						//std::cerr << std::strerror(errno) << std::endl;
						continue;
					}
				}
				if (Server::clients.size() >= 3)
				{
					tintin->writeLog(("[ERROR] Max clients reached, rejecting fd ") + std::to_string(clientFd));
					close(clientFd);
				}
				else
				{
					if (fcntl(clientFd, F_SETFL, O_NONBLOCK) != 0)
					{
						tintin->writeLog("[ERROR] fcntl error");
						
					}
					event.events = EPOLLIN | EPOLLET;
					event.data.fd = clientFd;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &event) < 0) {
						tintin->writeLog("[ERROR] cannot add client " + std::to_string(clientFd));
						close(clientFd);
						continue;
					}
					else
					{
						tintin->writeLog(("[INFO] client accepted fd ") + std::to_string(clientFd));
						Server::clients.insert(clientFd);
					}
				}
			}
			else {
					int clientFd = events[i].data.fd;
					char buffer[1024];
					while (true)
					{
						ssize_t n = recv(clientFd, buffer, sizeof(buffer), 0);

						if (n > 0)
						{
							std::string msg(buffer, n);
							
							if (msg == "quit")
							{
								Server::sutDown(socket_fd, epoll_fd);
							}
							
							else
							{
								tintin->writeLog("[LOG]" + msg);
							}
						}
						else if (n == -1 && errno == EAGAIN)
							break;
						else if (n == 0)
						{
							if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientFd, nullptr) < 0)
							{
								tintin->writeLog("[ERROR] error EPOLL_CTL_DEL");
							}
							else {
								close (clientFd);
								Server::clients.erase(clientFd);
								break;
							}
						}
					}
				}
		}
	}
	close(socket_fd);
	close(epoll_fd);
	return;
}

void	Server::createFile()
{
	const char* path = "/var/lock/matt_daemon.lock";

	int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0644);

	if (fd < 0)
	{
		std::cerr << "ERROR CREATING THE FILE" << std::endl;
		exit(EXIT_FAILURE);
	}
	close(fd);
}