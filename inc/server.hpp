#ifndef SERVER_HPP
# define SERVER_HPP

#include "includes.hpp"

class Server {
	private:
		int					port;
		struct sockaddr_in	servAddr;
		int					socket_fd;
	public:
		Server();
		Server(const Server &copy);
		Server& operator=(const Server &copy);
		~Server();
		void		runServer();
		
};

#endif