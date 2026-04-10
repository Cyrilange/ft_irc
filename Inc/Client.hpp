
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "CommandHandler.hpp"


class Client
{
private:
    int _fd;                 // file descriptor
    std::string _buffer;     // buffer for partial messages
    std::string _nick;       // nickname
    std::string _username;   // username
    std::string _realname;   //realname
	bool _passAccepted;     //password yes or not 
	bool _welcomeSent;
    bool        _hasPing;

public:
    Client(int fd);
    ~Client();

    int getFd() const;
	void setPassAccepted(bool value);
	bool isPassAccepted() const;
    void appendToBuffer(const std::string &data);
    std::string extractMessage();
	void sendMessage(const std::string& msg);
    void setNick(const std::string &nick);
    std::string getNick() const;
	bool isRegistered() const; //client check PASS + NICK + USER
    void setUsername(const std::string &username);
    void setRealname(const std::string &realname);
    std::string getUsername() const;
	bool isWelcomeSent() const;
	void setWelcomeSent(bool v);
};

#endif