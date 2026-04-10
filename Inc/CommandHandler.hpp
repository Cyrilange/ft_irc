/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 13:12:54 by csalamit          #+#    #+#             */
/*   Updated: 2026/04/10 16:13:40 by csalamit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "Server.hpp"
#include "Replies.hpp"

class Bot; 
class Server;
class Client;
class Channel;
class Commandhandler;



class CommandHandler
{
public:
    CommandHandler(Server* server);
    ~CommandHandler();

    typedef void (CommandHandler::*HandlerFunc)(Client*, std::istringstream&);

    void handleCommand(Client* client, std::string msg);
    void handleCAP(Client* client, std::istringstream& iss);
    void handlePASS(Client* client, std::istringstream& iss);
    void handleNICK(Client* client, std::istringstream& iss);
    void handleUSER(Client* client, std::istringstream& iss);
    void handleJOIN(Client* client, std::istringstream& iss);
    void handlePRIVMSG(Client* client, std::istringstream& iss);
    void handlePING(Client* client, std::istringstream& iss);
    void handleQUIT(Client* client, std::istringstream& iss);
    void handlePART(Client* client, std::istringstream& iss);
    void handleMODE(Client* client, std::istringstream& iss);
    void handleTOPIC(Client* client, std::istringstream& iss);
    void handleKICK(Client *client, std::istringstream& iss);
    void handleINVITE(Client *client, std::istringstream& iss);
    void handleWHO(Client *client, std::istringstream& iss);
    void handleNOTICE(Client* client, std::istringstream& iss);
    void sendWelcome(Client* client);

    Bot getBot();
   

private:
    Server* _server;
    std::map<std::string, HandlerFunc> _handlers;
    void initHandlers();
    std::vector<ModeChange> parseModeString(const std::string& modeStr, std::istringstream& iss);
    Bot _bot;
  
};

#endif