#include "inc/includes.hpp"
#include "inc/server.hpp"
#include "inc/tintin.hpp"

void my_daemonize()
{
	pid_t	pid;

	pid = fork();

	if (pid < 0) //error
		exit(EXIT_FAILURE);

	if (pid > 0) //success
		exit(EXIT_SUCCESS);
	
	if (setsid() < 0) //if < 0 fail, on success the child process becomes the session leader
		exit(EXIT_FAILURE);
	
	//SIGNALHANDLER

	pid = fork();

	if (pid < 0)
		exit(EXIT_FAILURE);
	
	if (pid > 0)
		exit(EXIT_SUCCESS);
	
	umask(0); //permissions
	
	if (chdir("/") < 0)
		exit(EXIT_FAILURE);

	for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
	{
		close(fd);
	}
}

int main()
{
	if (getuid() != 0)
	{
		fatalError("YOU NEED ROOT PERMISSIONS");
	}
	my_daemonize();
	Server server;
	server.createFile();
	server.runServer();
	return 0;
}