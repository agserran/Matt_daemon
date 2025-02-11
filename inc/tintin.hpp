#ifndef TINTIN_HPP
# define TINTIN_HPP

#include "includes.hpp"

class Tintin{
	private:
		std::string		logName;
	public:
		Tintin();
		~Tintin();
		Tintin(const Tintin &copy);
		Tintin& operator=(const Tintin &copy);
		std::string &getDate() const;
		std::string	getLogName() const;
		void	checker() const;
		void	writeLog(const std::string &message) const;
};

#endif