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
	delete tintin;
	//log de cerrado
}

void	Server::runServer()
{
	int	clientFd;
	int	epoll_fd = epoll_create(10);
	struct epoll_event event, events[MAX_EVENTS];
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
	{
		std::cerr << "Error: epoll error" << std::endl;
		close(socket_fd);
		close(epoll_fd);
		return ;
	}

	while (true)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

		try{
            if (num_events == -1) {// Si hay un error en epoll_wait lanzamos una excepción
                throw ErrorHandler::SocketEpoll();
            }
        }
        catch (std::exception &e){
            std::cerr << e.what() << std::endl;
            return;
        }
		for (int i = 0; i < num_events; i++)
		{
			if (events[i].data.fd == socket_fd)
			{
				struct sockaddr_in clientAddr;
				socklen_t addrlen = sizeof(clientAddr);
				if (Server::clients.size() < 3)
				{
					int clientFd = accept(socket_fd, (struct sockaddr*)&clientAddr, &addrlen);
					if (clientFd == -1)
					{
						std::cerr << "Error: cannot add client" << std::endl;
						continue;
					}
					if (Server::clients.size() >= 3)
					{
						tintin->writeLog(("[ERROR] Max clients reached, rejecting fd ") + clientFd);
						close(clientFd);
					}
					else
					{
						fcntl(clientFd, F_SETFL, O_NONBLOCK);
						event.events = EPOLLIN | EPOLLET;
						event.data.fd = clientFd;
						if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
							std::cerr << "Error: cannot add client" << std::endl;
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
			}
			else {
					int clientFd = events[i].data.fd;
					char buffer[1024];
					ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0);

					if (bytes <= 0)
					{
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientFd, nullptr);
						close (clientFd);
						Server::clients.erase(clientFd);
					}
					else
					{
						std::string msg(buffer);

						if (msg == "quit")
						{
							//cerrar servidor
							for (int fd : Server::clients)
							{
								epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
								close(fd);
								break;
							}
							tintin->writeLog("[INFO] Server closed");
						}

						else
						{
							tintin->writeLog(msg);
						}
					}
				}
		}
	}
	close(socket_fd);
	close(epoll_fd);
	return;
}
