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

Server &Server::operator=(const Server &other)
{
	delete tintin;
	port = other.port;
	servAddr = other.servAddr;
	socket_fd = other.socket_fd;
	clients = other.clients;
	tintin = new Tintin(*other.tintin);
	return *this;
}

Server::~Server()
{
	delete tintin;
}

void Server::unlinkFile()
{
	if (this->lock_fd >= 0)
	{
		flock(this->lock_fd, LOCK_UN);
		close(this->lock_fd);
		if (unlink("/var/lock/matt_daemon.lock") < 0)
		{
			tintin->writeLog("[ERROR] unlink");
		}
	}
}

void Server::shutDown()
{
	tintin->writeLog("[INFO] Server closed");
	for (int fd : Server::clients)
	{
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
		close(fd);
	}
	Server::clients.clear();
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, nullptr);
	close(socket_fd);
	close(epoll_fd);
	unlinkFile();
}

void	Server::statusChecker()
{
	switch (sig) {
		case SIGINT:
			tintin->writeLog("[INFO] SIGINT CAUGHT");
			this->shutDown();
			break;
		
		case	SIGTERM:
			tintin->writeLog("[INFO] SIGTERM CAUGHT");
			this->shutDown();
			break;
		
		case	SIGQUIT:
			tintin->writeLog("[INFO] SIGQUIT CAUGHT");
			this->shutDown();
			signal(SIGQUIT, SIG_DFL);
			raise(SIGQUIT);
			break;
	}
}

void Server::runServer()
{
	epoll_fd = epoll_create(10);
	if (epoll_fd == -1)
	{
		tintin->writeLog("[ERROR] error epoll");
		exit(EXIT_FAILURE);
	}
	struct epoll_event event, events[MAX_EVENTS];
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = socket_fd;
	int flags = fcntl(socket_fd, F_GETFL, 0);
	if (flags < 0)
	{
		tintin->writeLog("[ERROR] F_GETFL.");
		exit(EXIT_FAILURE);
	}
	if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		tintin->writeLog("[ERROR] fcntl error");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

	if (bind(socket_fd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	{
		tintin->writeLog(std::string("[ERROR] bind failed"));
		exit(EXIT_FAILURE);
	}

	if (listen(socket_fd, MAX_CLIENTS) < 0)
	{
		tintin->writeLog(std::string("[ERROR] listen failed"));
		exit(EXIT_FAILURE);
	}

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
	{
		tintin->writeLog("[ERROR] error creating socker fd");
		close(socket_fd);
		close(epoll_fd);
		exit(EXIT_FAILURE);
	}
	tintin->writeLog("[INFO] server created");
	while (true)
	{
		if (sig != 0)
		{
			this->statusChecker();
			return;
		}	
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

		if (num_events < 0)
		{
			if (errno == EINTR)
			{
				if (sig != 0)
				{
					this->statusChecker();
					return;
				}
				continue;
			}
			else
			{
				tintin->writeLog("[ERROR] epoll_wait");
			}	
			break;
		}
		
		for (int i = 0; i < num_events; i++)
		{
			if (events[i].data.fd == socket_fd)
			{
				struct sockaddr_in clientAddr;
				socklen_t addrlen = sizeof(clientAddr);
				int clientFd = accept(socket_fd, (struct sockaddr *)&clientAddr, &addrlen);
				if (clientFd == -1)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break;
					else
					{
						tintin->writeLog("Error: accept failed");
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
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &event) < 0)
					{
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
			else
			{
				int clientFd = events[i].data.fd;
				char buffer[1024];
				while (true)
				{
					ssize_t n = recv(clientFd, buffer, sizeof(buffer), 0);

					if (n > 0)
					{
						std::string msg(buffer, n);
						while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
							msg.pop_back();
						if (msg == "quit")
						{
							Server::shutDown();
							return;
						}

						else
						{
							tintin->writeLog("[LOG] " + msg);
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
						else
						{
							close(clientFd);
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

static const std::string readFirtsLine()
{
	std::ifstream file("/var/lock/matt_daemon.lock");
	if (!file.is_open())
		fatalError("CANNOT ACCESS TO LOCK FILE");
	std::string line;
	if (std::getline(file, line))
		return line;
	//fatalError("INVALID LOCKFILE");
	return std::string{};
}

void	checkPID()
{
	pid_t pid = getpid();
	std::string	lockPID = readFirtsLine();
	if (std::to_string(pid) != lockPID)
		fatalError("INVALID LOCKFILE, DIFERENT PID.");
}

void Server::createFile()
{
	const char *path = "/var/lock/matt_daemon.lock";
	if (access(path, F_OK) == 0)
	{
		tintin->writeLog("[ERROR] ONE DAEMON INSTANCE ALREADY RUNNING");
		exit(EXIT_FAILURE);
	}

	this->lock_fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0644);
	std::string pid = std::to_string(getpid());
	if (write(lock_fd, pid.data(), pid.size()) != (ssize_t)pid.size())
	{
		close(lock_fd);
		fatalError("CANNOT WRITE PID");
	}

	if (this->lock_fd < 0)
	{
		fatalError("CANNOT CREATE THE LO FILE");
	}

	if (flock(this->lock_fd, LOCK_EX | LOCK_NB) < 0)
	{
		fatalError("THERE IS ALREADY AN INSTANCE RUNNING");
		close(this->lock_fd);
	}
	checkPID();
}
