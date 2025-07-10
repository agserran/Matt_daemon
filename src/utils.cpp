#include "../inc/includes.hpp"

volatile sig_atomic_t sig = 0;


void	fatalError(const std::string what)
{
	std::cerr << "[FATAL] " << what << std::endl;
	std::exit(EXIT_FAILURE);
}

void	signalHandle(int sign)
{
	sig = sign;
}