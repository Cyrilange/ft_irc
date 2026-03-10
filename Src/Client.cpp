/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 01:32:50 by csalamit          #+#    #+#             */
/*   Updated: 2026/03/10 15:48:00 by csalamit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/Client.hpp"


Client::Client(int fd) : _fd(fd) , _passAccepted(false) , _welcomeSent(false) {}
Client::~Client() {}
int Client::getFd() const { return _fd; }
void Client::appendToBuffer(const std::string &data) { _buffer += data; }

std::string Client::extractMessage()
{
    size_t pos = _buffer.find("\r\n");

    if (pos == std::string::npos)
        pos = _buffer.find('\n');

    if (pos == std::string::npos)
        return "";

    std::string message = _buffer.substr(0, pos);

    if (_buffer[pos] == '\r')
        _buffer.erase(0, pos + 2);
    else
        _buffer.erase(0, pos + 1);

    return message;
}

void Client::setNick(const std::string &nick) { _nick = nick; }
std::string Client::getNick() const { return _nick; }
void Client::setUsername(const std::string &username) { _username = username; }
std::string Client::getUsername() const { return _username; }
void Client::setPassAccepted(bool value) { _passAccepted = value; }
bool Client::isPassAccepted() const { return _passAccepted;}
bool Client::isRegistered() const { return _passAccepted && !_nick.empty() && !_username.empty();}
void Client::sendMessage(const std::string& msg) { ::send(_fd, msg.c_str(), msg.length(), 0);}
bool Client::isWelcomeSent() const { return _welcomeSent; }
void Client::setWelcomeSent(bool v) { _welcomeSent = v; }


/*
** Client.cpp
**
** WHAT IS DONE:
** - Constructor initializes fd, _passAccepted and _welcomeSent
** - extractMessage() handles both \r\n and \n line endings
** - appendToBuffer() accumulates incoming data until full message received
** - isRegistered() checks PASS + NICK + USER all set before welcome
** - sendMessage() sends raw message directly to client fd
** - isWelcomeSent() / setWelcomeSent() prevents sending 001 twice
**
** WHAT IS MISSING:
** - _channels : list of channels the client is in (needed for QUIT/PART broadcast)
** - addChannel()    : add a channel to client's list when he joins
** - removeChannel() : remove a channel from client's list when he leaves
** - getChannels()   : return list of channels for QUIT broadcast
** - _hostname       : store client hostname (needed for proper IRC message format)
** - _realname       : store realname from USER command (4th parameter)
** - getPrefix()     : return ":nick!user@host" format used in all IRC messages
** - sendMessage()   : should add \r\n protection like sendResponse() in CommandHandler
** - Destructor      : should remove client from all channels before destroying
*/  
