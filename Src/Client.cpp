/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 01:32:50 by csalamit          #+#    #+#             */
/*   Updated: 2026/03/09 22:40:54 by csalamit         ###   ########.fr       */
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
void Client::sendMessage(const std::string& msg) { ::send(_fd, msg.c_str(), msg.length(), 0); }
bool Client::isWelcomeSent() const { return _welcomeSent; }
void Client::setWelcomeSent(bool v) { _welcomeSent = v; }
