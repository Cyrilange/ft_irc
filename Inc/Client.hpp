/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 01:30:17 by csalamit          #+#    #+#             */
/*   Updated: 2026/03/08 12:52:31 by csalamit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
private:
    int _fd;                 // file descriptor
    std::string _buffer;     // buffer for partial messages
    std::string _nick;       // nickname
    std::string _username;   // username
	bool _passAccepted;     //password yes or not 

public:
    Client(int fd);
    ~Client();

    int getFd() const;
	void setPassAccepted(bool value);
	bool isPassAccepted() const;
    void appendToBuffer(const std::string &data);
    std::string extractMessage();

    void setNick(const std::string &nick);
    std::string getNick() const;

    void setUsername(const std::string &username);
    std::string getUsername() const;
};

#endif