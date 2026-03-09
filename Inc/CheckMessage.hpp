#ifndef CHECKMESSAGE_HPP
# define CHECKMESSAGE_HPP


#include <sstream>
#include <iostream>
# include <string>
#include <set>

class CheckMessage {
	public:
	CheckMessage(void);
	static bool isComandValid(const std::string& command);
	~CheckMessage(void);

};

#endif 