#include "inc/includes.hpp"

int main()
{
	std::time_t now = std::time(nullptr);
	std::tm* localTime = std::localtime(&now);

	std::string ret = "[ " + std::to_string(localTime->tm_mday) + " / "
              + std::to_string(localTime->tm_mon + 1) + " / " 
              + std::to_string(localTime->tm_year + 1900) + " - "
              + std::to_string(localTime->tm_hour) + " : " 
              + std::to_string(localTime->tm_min) + " : " 
              + std::to_string(localTime->tm_sec) + " ]";


	std::cout << ret << std::endl;
	return (0);
}