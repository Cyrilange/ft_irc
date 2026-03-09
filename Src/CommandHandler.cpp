#include "../Inc/CommandHandler.hpp"
#include "../Inc/Server.hpp"
#include "../Inc/Client.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server* server) : _server(server) { initHandlers(); } // Constructor: store server and init commands

CommandHandler::~CommandHandler() {} // Destructor: nothing special to clean

void CommandHandler::initHandlers() { // Initialize command map (string -> function)
    _handlers["CAP"] = &CommandHandler::handleCAP; //cap is for hexchat 
    _handlers["PASS"] = &CommandHandler::handlePASS; // PASS command handler
    _handlers["NICK"] = &CommandHandler::handleNICK; // NICK command handler
    _handlers["USER"] = &CommandHandler::handleUSER; // USER command handler
    _handlers["JOIN"] = &CommandHandler::handleJOIN; // JOIN command handler
    _handlers["PRIVMSG"] = &CommandHandler::handlePRIVMSG; // PRIVMSG handler
    _handlers["MODE"] = &CommandHandler::handleMODE;
}

void CommandHandler::handleCAP(Client* client, std::istringstream& iss)
{
    std::string subcmd;
    iss >> subcmd;

    if (subcmd == "LS") {
        std::string response = ":ircserv CAP * LS :\r\n";
        send(client->getFd(), response.c_str(), response.length(), 0);
    } else if (subcmd == "REQ") {
        std::string response = ":ircserv CAP * NAK :\r\n";
        send(client->getFd(), response.c_str(), response.length(), 0);
    }
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

void CommandHandler::sendWelcome(Client* client)
{
    std::string nick = client->getNick();
    std::string msg;

    msg = ":ircserv 001 " + nick + " :Welcome to the IRC Network " + nick + "\r\n";
    send(client->getFd(), msg.c_str(), msg.length(), 0);

    msg = ":ircserv 002 " + nick + " :Your host is ircserv, running version 1.0\r\n";
    send(client->getFd(), msg.c_str(), msg.length(), 0);

    msg = ":ircserv 003 " + nick + " :This server was created just now\r\n";
    send(client->getFd(), msg.c_str(), msg.length(), 0);

    msg = ":ircserv 004 " + nick + " ircserv 1.0 o o\r\n";
    send(client->getFd(), msg.c_str(), msg.length(), 0);
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

    if (client->isRegistered() && !client->isWelcomeSent())
    {
        client->setWelcomeSent(true);
        sendWelcome(client);
    }

    client->setNick(nick); // Set nickname for client
    std::cout << "Client fd=" << client->getFd() << " nick=" << nick << std::endl; // Debug
}

void CommandHandler::handleUSER(Client* client, std::istringstream& iss)
{
    std::string username; // Variable for username
    iss >> username; // Get username
    client->setUsername(username); // Set username
    if (client->isRegistered() && !client->isWelcomeSent())
    {
        client->setWelcomeSent(true);
        sendWelcome(client);
    }
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

std::vector<ModeChange> CommandHandler::parseModeString(const std::string& modeStr, std::istringstream& iss)
{
    std::vector<ModeChange> changes;
    if (modeStr.empty()) {return changes; }
    char sign = '+';// sign per default
    for (size_t i = 0; i < modeStr.size(); i++)
    {
        if (modeStr[i] == '+' || modeStr[i] == '-') { sign = modeStr[i]; continue;}
        ModeChange m;
        m.sign = sign;
        m.mode = modeStr[i];
        m.param = "";
        if (m.mode == 'k' || m.mode == 'o' || m.mode == 'l')
            iss >> m.param;
        changes.push_back(m);
    }

    return changes;
}

void CommandHandler::handleMODE(Client* client, std::istringstream& iss)
{
    std::string target;   // channel or nick
    std::string modeStr;  // ex: "+itk" ou "-o"
    iss >> target >> modeStr;
    (void)client;

    std::vector<ModeChange> changes = parseModeString(modeStr, iss);

    for (size_t i = 0; i < changes.size(); i++)
    {
        ModeChange& m = changes[i];
        if (m.mode == 'i') { /* invite only */ }
        else if (m.mode == 't') { /* topic restrict */ }
        else if (m.mode == 'k') { /* password, m.param = password */ }
        else if (m.mode == 'o') { /* op, m.param = nick */ }
        else if (m.mode == 'l') { /* limit, m.param = number */ }
    }
}