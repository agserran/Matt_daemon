#include "../inc/includes.hpp"

void	fatalError(const std::string what)
{
	std::cerr << "[FATAL] " << what << std::endl;
	std::exit(EXIT_FAILURE);
}