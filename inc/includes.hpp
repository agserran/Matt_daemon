#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <fcntl.h>
#include <netinet/ip.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

extern volatile sig_atomic_t sig;

void fatalError(const std::string what);
void signalHandle(int sign);

#endif