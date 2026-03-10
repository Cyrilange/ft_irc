#include "../Inc/CommandHandler.hpp"
#include "../Inc/Server.hpp"
#include "../Inc/Client.hpp"
#include "../Inc/Replies.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server* server) : _server(server) { initHandlers(); } // Constructor: store server pointer and initialize command handlers

CommandHandler::~CommandHandler() {} // Destructor: nothing to clean

void CommandHandler::initHandlers() {
    _handlers["CAP"]     = &CommandHandler::handleCAP;     // CAP command for HexChat handshake
    _handlers["PASS"]    = &CommandHandler::handlePASS;    // PASS command for authentication
    _handlers["NICK"]    = &CommandHandler::handleNICK;    // NICK command to set nickname
    _handlers["USER"]    = &CommandHandler::handleUSER;    // USER command to set username
    _handlers["JOIN"]    = &CommandHandler::handleJOIN;    // JOIN command to join a channel
    _handlers["PRIVMSG"] = &CommandHandler::handlePRIVMSG; // PRIVMSG command to send a message
    _handlers["MODE"]    = &CommandHandler::handleMODE;    // MODE command to change channel modes
}

static void sendResponse(Client* client, const std::string& msg) // Send a formatted response to client, adds \r\n if missing
{
    std::string formatted = msg;
    if (formatted.length() < 2 || formatted.substr(formatted.length() - 2) != "\r\n") // Check if \r\n is missing
        formatted += "\r\n";                                                            // Add \r\n if missing
    send(client->getFd(), formatted.c_str(), formatted.length(), 0);                   // Send to client
}

static void sendError(Client* client, const std::string& code, const std::string& message) // Send a numeric error reply to client
{
    std::string nick = client->getNick().empty() ? "*" : client->getNick(); // Use * if nick not set yet
    sendResponse(client, ":ircserv " + code + " " + nick + " " + message);  // Format and send error
}

void CommandHandler::handleCAP(Client* client, std::istringstream& iss) // Handle CAP negotiation for HexChat
{
    std::string subcmd;
    iss >> subcmd;                                              // Extract CAP subcommand (LS, REQ, END)

    if (subcmd == "LS")                                         // Client requests list of capabilities
        sendResponse(client, ":ircserv CAP * LS :");            // Reply with empty capabilities list
    else if (subcmd == "REQ")                                   // Client requests a capability
        sendResponse(client, ":ircserv CAP * NAK :");           // Reject all capability requests
}

void CommandHandler::handleCommand(Client* client, std::string msg) // Parse and dispatch incoming IRC command
{
    std::istringstream iss(msg);                                // Create stream from raw message
    std::string cmd;                                            // Variable to store command name
    iss >> cmd;                                                 // Extract first word (the command)
    cmd.erase(cmd.find_last_not_of("\r\n") + 1);               // Strip trailing \r\n from command

    // CAP, NICK, USER allowed before PASS for HexChat handshake
    if (!client->isPassAccepted() && cmd != "PASS" && cmd != "CAP" && cmd != "NICK" && cmd != "USER")
    {
        sendResponse(client, "ERROR :You need to authenticate with PASS first"); // Reject unauthenticated command
        return;
    }

    std::map<std::string, HandlerFunc>::iterator it = _handlers.find(cmd); // Look up command in handler map
    if (it != _handlers.end())                                              // If command exists
        (this->*(it->second))(client, iss);                                 // Call the corresponding handler
    else
        std::cout << "Unknown command: " << cmd << std::endl;               // Debug: unknown command
}

void CommandHandler::sendWelcome(Client* client) // Send the 4 welcome messages after successful registration
{
    const std::string& nick = client->getNick(); // Get client nickname

    sendResponse(client, ":ircserv " + std::string(RPL_WELCOME)  + " " + nick + " :Welcome to the IRC Network " + nick); // 001
    sendResponse(client, ":ircserv " + std::string(RPL_YOURHOST) + " " + nick + " :Your host is ircserv, running version 1.0"); // 002
    sendResponse(client, ":ircserv " + std::string(RPL_CREATED)  + " " + nick + " :This server was created just now"); // 003
    sendResponse(client, ":ircserv " + std::string(RPL_MYINFO)   + " " + nick + " ircserv 1.0 o o"); // 004
}

void CommandHandler::handlePASS(Client* client, std::istringstream& iss) // Handle PASS command for authentication
{
    std::string pass;
    iss >> pass;                                                // Extract password from message

    if (client->isPassAccepted()) {                             // If already authenticated
        sendError(client, ERR_ALREADYREGISTRED, ":You may not reregister"); // Send already registered error
        return;
    }

    if (pass == _server->getPassword()) {                       // If password matches
        client->setPassAccepted(true);                          // Mark client as authenticated
        std::cout << "Client " << client->getFd() << " password accepted" << std::endl; // Debug
    } else {                                                    // If password is wrong
        sendError(client, ERR_PASSWDMISMATCH, ":Password incorrect"); // Send wrong password error
    }
}

static bool isValidNickChar(char c, bool first) // Check if character is valid for a nickname
{
    if (first) // First character must be a letter or special character
        return (std::isalpha(c) || c == '[' || c == ']' || c == '\\' ||
                c == '`' || c == '_' || c == '^' || c == '{' || c == '|' || c == '}');
    return (std::isalpha(c) || std::isdigit(c) || c == '-' || // Other characters can be letter, digit or special
            c == '[' || c == ']' || c == '\\' || c == '`' ||
            c == '_' || c == '^' || c == '{' || c == '|' || c == '}');
}

static bool isValidNick(const std::string& nick) // Check if the full nickname is valid
{
    if (nick.empty() || nick.length() > 9)        // Nick must be 1-9 characters (IRC standard)
        return false;
    if (!isValidNickChar(nick[0], true))           // First character check
        return false;
    for (size_t i = 1; i < nick.size(); i++) {     // Check remaining characters
        if (!isValidNickChar(nick[i], false))
            return false;
    }
    return true;
}

void CommandHandler::handleNICK(Client* client, std::istringstream& iss) // Handle NICK command to set or change nickname
{
    std::string nick;
    iss >> nick;                                                // Extract nickname from message

    if (nick.empty()) {                                         // If no nickname provided
        sendError(client, ERR_NONICKNAMEGIVEN, ":No nickname given"); // Send missing nick error
        return;
    }

    if (nick[nick.size() - 1] == '\r') { nick.erase(nick.size() - 1);}                         // Remove trailing \r if present
    if (!isValidNick(nick)) {                                       // If nickname contains invalid characters
        sendError(client, ERR_ERRONEUSNICK, nick + " :Erroneous nickname"); // Send invalid nick error
        return;
    }

    std::vector<Client*>& clients = _server->getClients();      // Get list of all connected clients
    for (size_t i = 0; i < clients.size(); i++) {               // Loop through all clients
        if (clients[i]->getNick() == nick) {                    // If nick already taken
            sendError(client, ERR_NICKNAMEINUSE, nick + " :Nickname is already in use"); // Send nick in use error
            return;
        }
    }

    client->setNick(nick);                                      // Set nick before checking registration
    std::cout << "Client fd=" << client->getFd() << " nick=" << nick << std::endl; // Debug

    if (client->isRegistered() && !client->isWelcomeSent()) {   // If fully registered and welcome not sent yet
        client->setWelcomeSent(true);                           // Mark welcome as sent
        sendWelcome(client);                                    // Send welcome messages
    }
}

void CommandHandler::handleUSER(Client* client, std::istringstream& iss) // Handle USER command to set username
{
    std::string username;
    iss >> username;                                            // Extract username from message
    client->setUsername(username);                              // Set username on client
    std::cout << "Client " << client->getFd() << " set username: " << username << std::endl; // Debug

    if (client->isRegistered() && !client->isWelcomeSent()) {   // If fully registered and welcome not sent yet
        client->setWelcomeSent(true);                           // Mark welcome as sent
        sendWelcome(client);                                    // Send welcome messages
    }
}

void CommandHandler::handleJOIN(Client* client, std::istringstream& iss) // Handle JOIN command to join a channel
{
    std::string channelName;
    iss >> channelName;                                         // Extract channel name from message
    std::cout << "Client " << client->getFd() << " joining channel: " << channelName << std::endl; // Debug
}

void CommandHandler::handlePRIVMSG(Client* client, std::istringstream& iss) // Handle PRIVMSG command to send a message
{
    std::string target;
    iss >> target;                                              // Extract target (nick or channel)
    std::string message;
    std::getline(iss, message);                                 // Get the rest as message content
    if (!message.empty() && message[0] == ' ')                  // Remove leading space if present
        message.erase(0, 1);

    client->sendMessage("Message to " + target + ": " + message + "\r\n"); // Send message (routing not implemented yet)
}

std::vector<ModeChange> CommandHandler::parseModeString(const std::string& modeStr, std::istringstream& iss) // Parse mode string into list of ModeChange structs
{
    std::vector<ModeChange> changes;
    if (modeStr.empty())                                        // If no mode string provided
        return changes;                                         // Return empty list

    char sign = '+';                                            // Default sign is +
    for (size_t i = 0; i < modeStr.size(); i++)
    {
        if (modeStr[i] == '+' || modeStr[i] == '-') { sign = modeStr[i]; continue; } // Update sign
        ModeChange m;
        m.sign = sign;                                          // Set sign for this mode
        m.mode = modeStr[i];                                    // Set mode letter
        m.param = "";                                           // Default no parameter
        if (m.mode == 'k' || m.mode == 'o' || m.mode == 'l')   // These modes require a parameter
            iss >> m.param;                                     // Extract parameter from stream
        changes.push_back(m);                                   // Add to list
    }
    return changes;
}

void CommandHandler::handleMODE(Client* client, std::istringstream& iss) // Handle MODE command to change channel modes
{
    std::string target;
    std::string modeStr;
    iss >> target >> modeStr;                                   // Extract target and mode string
    (void)client;                                               // Unused for now

    std::vector<ModeChange> changes = parseModeString(modeStr, iss); // Parse mode string into changes

    for (size_t i = 0; i < changes.size(); i++)
    {
        ModeChange& m = changes[i];
        if (m.mode == 'i') { /* invite only */ }                // Toggle invite-only mode
        else if (m.mode == 't') { /* topic restrict */ }        // Toggle topic restriction
        else if (m.mode == 'k') { /* password */ }              // Set/remove channel password
        else if (m.mode == 'o') { /* operator */ }              // Give/take operator privilege
        else if (m.mode == 'l') { /* user limit */ }            // Set/remove user limit
    }
}

/*
** CommandHandler.cpp
**
** WHAT IS DONE:
** - CAP handshake for HexChat (LS, REQ, NAK)
** - PASS authentication with error replies
** - NICK with duplicate check and error replies
** - USER with username set
** - Welcome messages (001, 002, 003, 004) on full registration
** - MODE string parser (sign + mode + param)
** - sendResponse() and sendError() utilities
**
** WHAT IS MISSING:
** - QUIT   : disconnect client and broadcast to all channels he was in
** - PART   : leave a channel and broadcast to remaining members
** - PING   : respond with PONG or HexChat will disconnect after timeout
** - KICK   : operator removes a client from a channel (needs Channel class)
** - INVITE : operator invites a client to invite-only channel (needs Channel class)
** - TOPIC  : view or change channel topic (needs Channel class)
** - MODE   : handlers are empty, need Channel class to apply i/t/k/o/l
** - JOIN   : needs Channel class to create/join, broadcast, send 353/366
** - PRIVMSG: needs routing to channel members or specific nick
**
** CHANNEL CLASS IS REQUIRED FOR:
** - JOIN, PART, KICK, INVITE, TOPIC, MODE, PRIVMSG
** - Channel must store: name, topic, key, userLimit,
**   inviteOnly, topicRestricted, members, operators
**
** BONUS MISSING:
** - IRC Bot responding to commands in a channel (!time, !help, !echo)
** - File transfer via DCC SEND
*/