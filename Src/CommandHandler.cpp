#include "../Inc/CommandHandler.hpp"
#include "../Inc/Server.hpp"
#include "../Inc/Client.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server* server) : _server(server) { initHandlers(); } // Constructor: store server and init commands

CommandHandler::~CommandHandler() {} // Destructor: nothing special to clean

void CommandHandler::initHandlers() { // Initialize command map (string -> function)
    _handlers["PASS"] = &CommandHandler::handlePASS; // PASS command handler
    _handlers["NICK"] = &CommandHandler::handleNICK; // NICK command handler
    _handlers["USER"] = &CommandHandler::handleUSER; // USER command handler
    _handlers["JOIN"] = &CommandHandler::handleJOIN; // JOIN command handler
    _handlers["PRIVMSG"] = &CommandHandler::handlePRIVMSG; // PRIVMSG handler
}

void CommandHandler::handleCommand(Client* client, std::string msg)
{
    std::istringstream iss(msg); // Create stream to read words from message
    std::string cmd; // Variable to store command name
    iss >> cmd; // Extract first word (command)
    cmd.erase(cmd.find_last_not_of("\r\n") + 1); // Remove \r or \n at end of command

    if (!client->isPassAccepted() && cmd != "PASS") // If password not accepted and not PASS
    {
        std::string error = "ERROR :You need to authenticate with PASS first\r\n"; // Error message
        send(client->getFd(), error.c_str(), error.length(), 0); // Send error to client
        return; // Stop processing command
    }

    std::map<std::string, HandlerFunc>::iterator it = _handlers.find(cmd); // Find command in map
    if (it != _handlers.end()) { // If command exists
        (this->*(it->second))(client, iss); // Call corresponding function
    } else { // If command not found
        std::cout << "Unknown command: " << cmd << std::endl; // Debug message
    }
}

void CommandHandler::handlePASS(Client* client, std::istringstream& iss)
{
    std::string pass; // Variable for password
    iss >> pass; // Get password from message

    if (client->isPassAccepted()) { // If password already sent before
        std::string error = "ERROR :PASS already sent\r\n"; // Error message
        send(client->getFd(), error.c_str(), error.length(), 0); // Send error
        return; // Stop
    }

    if (pass == _server->getPassword()) { // If password is correct
        client->setPassAccepted(true); // Mark client as authenticated
        std::cout << "Client " << client->getFd() << " password accepted" << std::endl; // Debug
    } else { // If password is wrong
        std::string error = "ERROR :Wrong password\r\n"; // Error message
        send(client->getFd(), error.c_str(), error.length(), 0); // Send error
        std::cout << "Client " << client->getFd() << " wrong password" << std::endl; // Debug
    }
}

void CommandHandler::handleNICK(Client* client, std::istringstream& iss)
{
    std::string nick; // Variable for nickname
    iss >> nick; // Get nickname

    if (nick.empty()) { // If nickname is empty
        std::cout << "Client " << client->getFd() << " tried to set empty nick" << std::endl; // Debug
        return; // Stop
    }

    if (!nick.empty() && nick[nick.size() - 1] == '\r') // Remove trailing \r if present
        nick.erase(nick.size() - 1);

    std::vector<Client*>& clients = _server->getClients(); // Get list of clients
    for (size_t i = 0; i < clients.size(); i++) { // Loop through clients
        if (clients[i]->getNick() == nick) { // If nick already used
            std::cout << "Nick already in use: " << nick << std::endl; // Debug
            return; // Stop
        }
    }

    client->setNick(nick); // Set nickname for client
    std::cout << "Client fd=" << client->getFd() << " nick=" << nick << std::endl; // Debug
}

void CommandHandler::handleUSER(Client* client, std::istringstream& iss)
{
    std::string username; // Variable for username
    iss >> username; // Get username
    client->setUsername(username); // Set username
    std::cout << "Client " << client->getFd() << " set username: " << username << std::endl; // Debug
}

void CommandHandler::handleJOIN(Client* client, std::istringstream& iss)
{
    std::string channelName; // Variable for channel name
    iss >> channelName; // Get channel name
    std::cout << "Client " << client->getFd() << " joining channel: " << channelName << std::endl; // Debug
}

void CommandHandler::handlePRIVMSG(Client* client, std::istringstream& iss)
{
    std::string target; // Target user or channel
    iss >> target; // Get target
    std::string message; // Message content
    std::getline(iss, message); // Get rest of message
    if (!message.empty() && message[0] == ' ') { message.erase(0,1); } // Remove leading space

    client->sendMessage("Message to " + target + ": " + message + "\r\n"); // Send message (if method exists)
    std::cout << "Client " << client->getFd() << " sending message to "
              << target << ": " << message << std::endl; // Debug
}