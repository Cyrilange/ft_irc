/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 13:12:54 by csalamit          #+#    #+#             */
/*   Updated: 2026/03/24 22:48:34 by csalamit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <sstream>
#include <map>
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include <sys/socket.h>
#include "Channel.hpp"
#include <algorithm>

class Server;
class Client;
class Channel;
 
class CommandHandler
{

public:
    CommandHandler(Server* server);
    ~CommandHandler();
    
    typedef void (CommandHandler::*HandlerFunc)(Client*, std::istringstream&);
    void handleCommand(Client *client, std::string msg); 

    // Command implementations
    void handleCAP(Client* client, std::istringstream& iss);
    void handlePASS(Client* client, std::istringstream& iss);
    void handleNICK(Client* client, std::istringstream& iss);
    void handleUSER(Client* client, std::istringstream& iss);
    void handleJOIN(Client* client, std::istringstream& iss);
    void handlePRIVMSG(Client* client, std::istringstream& iss);
    void handlePING(Client* client, std::istringstream& iss);
    void handleQUIT(Client* client, std::istringstream& iss);
    void handlePART(Client* client, std::istringstream& iss);
    void sendWelcome(Client* client);
    void handleMODE(Client* client, std::istringstream& iss);

private:
    Server* _server;
    std::map<std::string, HandlerFunc> _handlers;
    void initHandlers();
    std::vector<ModeChange> parseModeString(const std::string& modeStr, std::istringstream& iss);
};

#endif