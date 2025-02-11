#include "tintin.hpp"

Tintin::Tintin(){}

Tintin::~Tintin(){}

Tintin::Tintin(const Tintin &copy)
{
	this->logName = copy.getLogName();
	*this = copy;
}

Tintin &Tintin::operator=(const Tintin &copy)
{
	this->logName = copy.getLogName();
	*this = copy;
	return (*this);
}

std::string &Tintin::getDate() const
{
	std::time_t now = std::time(nullptr);
	std::tm* localTime = std::localtime(&now);

	std::string ret = "[ " + std::to_string(localTime->tm_mday) + " / "
              + std::to_string(localTime->tm_mon + 1) + " / " 
              + std::to_string(localTime->tm_year + 1900) + " - "
              + std::to_string(localTime->tm_hour) + " : " 
              + std::to_string(localTime->tm_min) + " : " 
              + std::to_string(localTime->tm_sec) + " ]";
	
	return (ret);
}

void Tintin::checker() const
{
	std::string logPath = "/var/log/matt_daemon/matt_daemon.log";
	std::string	logDir = "/var/log/matt_daemon/";

	struct stat info;
	if (stat(logDir.c_str(), &info) != 0)
	{
		if (mkdir(logDir.c_str(), 0777) != 0)
		{
			std::cerr << "Error: couldn´t create the directory." << std::endl;
		}
	}
	if (chmod(logPath.c_str(), 775) != 0)
		std::cerr << "Error: chmod." << std::endl;
}


void Tintin::writeLog(const std::string &message) const
{
	std::string date = getDate();
	std::ofstream logFile("/var/log/matt_daemon/matt_daemon.log", std::ios::app);
	
	checker();
	if (!logFile)
		{
			std::cerr << "Error: couldn´t open logFile." << std::endl;
			return ;
		}
	logFile << date + " " + message + "\n";
	logFile.close();
}