#include "inc/includes.hpp"
#include "inc/server.hpp"
#include "inc/tintin.hpp"

int main()
{
	if (getuid() != 0)
	{
		std::cerr << "YOU NEED ROOT PERMISSIONS" << std::endl;
		exit(EXIT_FAILURE);
	}
	Server server;
	server.createFile();
	server.runServer();
	return 0;
}