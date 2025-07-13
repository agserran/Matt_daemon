#ifndef INCLUDES_HPP
# define INCLUDES_HPP

#include	<iostream>
#include	<string>
#include	<vector>
#include	<map>
#include	<iterator>
#include	<sys/epoll.h>
#include	<netinet/ip.h>
#include	<sys/wait.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<ctime>
#include	<fstream>
#include	<sys/stat.h>
#include	<exception>
#include	<fcntl.h>
#include	<set>
#include	<signal.h>
#include	<sys/file.h>
#include	<csignal>
#include	<signal.h>
#include	<atomic>

extern volatile sig_atomic_t	sig;

void	fatalError(const std::string what);
void	signalHandle(int sign);

#endif