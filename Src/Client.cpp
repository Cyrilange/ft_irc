/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 01:32:50 by csalamit          #+#    #+#             */
/*   Updated: 2026/03/08 12:53:56 by csalamit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/Client.hpp"


Client::Client(int fd) : _fd(fd) , _passAccepted(false)  {}
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