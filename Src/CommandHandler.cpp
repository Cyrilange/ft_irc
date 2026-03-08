#include "../Inc/CommandHandler.hpp"
#include "../Inc/Server.hpp"
#include "../Inc/Client.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server* server) : _server(server)
{
    initHandlers();
}

CommandHandler::~CommandHandler() {}

void CommandHandler::initHandlers()
{
    _handlers["PASS"] = &CommandHandler::handlePASS;
    _handlers["NICK"] = &CommandHandler::handleNICK;
    _handlers["USER"] = &CommandHandler::handleUSER;
    _handlers["JOIN"] = &CommandHandler::handleJOIN;
    _handlers["PRIVMSG"] = &CommandHandler::handlePRIVMSG;
}

void CommandHandler::handleCommand(Client* client, std::string msg)
{
    std::istringstream iss(msg);
    std::string cmd;
    iss >> cmd;

    if (!client->isPassAccepted() && cmd != "PASS")
    {
        std::cout << "Client " << client->getFd() << " must authenticate first" << std::endl;
        return;
    }

    std::map<std::string, HandlerFunc>::iterator it = _handlers.find(cmd);
    if (it != _handlers.end())
    {
        (this->*(it->second))(client, iss);
    }
    else
    {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}

// ---- Command implementations ----

void CommandHandler::handlePASS(Client* client, std::istringstream& iss)
{
    std::string pass;
    iss >> pass;
    if (pass == _server->getPassword())
    {
        client->setPassAccepted(true);
        std::cout << "Client " << client->getFd() << " password accepted" << std::endl;
    }
    else
    {
        std::cout << "Client " << client->getFd() << " wrong password" << std::endl;
    }
}

void CommandHandler::handleNICK(Client* client, std::istringstream& iss)
{
    std::string nick;
    iss >> nick;
    client->setNick(nick);
    std::cout << "Client " << client->getFd() << " set nick: " << nick << std::endl;
}

void CommandHandler::handleUSER(Client* client, std::istringstream& iss)
{
    std::string username;
    iss >> username;
    client->setUsername(username);
    std::cout << "Client " << client->getFd() << " set username: " << username << std::endl;
}

void CommandHandler::handleJOIN(Client* client, std::istringstream& iss)
{
    std::string channelName;
    iss >> channelName;
    std::cout << "Client " << client->getFd() << " joining channel: " << channelName << std::endl;
    // TODO: Ajouter logique de création/join de Channel
}

void CommandHandler::handlePRIVMSG(Client* client, std::istringstream& iss)
{
    std::string target;
    iss >> target;
    std::string message;
    std::getline(iss, message);
    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);

    std::cout << "Client " << client->getFd() << " sending message to "
              << target << ": " << message << std::endl;
    // TODO: Logique pour envoyer message à client ou channel
}