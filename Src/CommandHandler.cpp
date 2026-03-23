
#include "../Inc/Server.hpp"
#include "../Inc/Client.hpp"
#include "../Inc/Replies.hpp"
#include "../Inc/Channel.hpp"
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
    _handlers["PING"] = &CommandHandler::handlePING;        //ping for hexChat
    _handlers["QUIT"] = &CommandHandler::handleQUIT;       //to quit , to leave 
    //part      
    //kick
    //topic
    //invite
    //who
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

void CommandHandler::handlePING(Client* client, std::istringstream& iss) // Handle PING to keep connection alive
{
    std::string token;
    iss >> token;                                               // Extract ping token sent by client

    if (token.empty())                                          // If no token provided
        sendResponse(client, ":ircserv PONG ircserv");          // Send basic pong
    else
        sendResponse(client, ":ircserv PONG ircserv :" + token); // Echo token back to client
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
    if (!client->isPassAccepted() && cmd != "PASS" && cmd != "CAP" && cmd != "NICK" && cmd != "USER" && cmd != "PING")
    {
        sendResponse(client, "ERROR :You need to authenticate with PASS first"); // Reject unauthenticated command
        return;
    }
    std::map<std::string, HandlerFunc>::iterator it = _handlers.find(cmd);

    if (it == _handlers.end()) {
        std::cout << "Unknown command: " << cmd << std::endl; // if you do like NICKnickname with no space bwteen the command and the name youll have an error on the server side 
        return;
    }   
    (this->*(it->second))(client, iss);                                 // Call the corresponding handler     
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
    if (nick[nick.size() - 1] == '\r') { nick.erase(nick.size() - 1);}                        
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

    //need to make nickname to be changed 
}

void CommandHandler::handleUSER(Client* client, std::istringstream& iss) // Handle USER command to set username
{
    if (client->isWelcomeSent()) {
        sendError(client, ERR_ALREADYREGISTRED, ":You may not reregister");
        return;
    }
    std::string username;
    iss >> username;                                            // Extract username from message
    client->setUsername(username);                              // Set username on client
    std::cout << "Client " << client->getFd() << " set username: " << username << std::endl; // Debug
    if (client->isRegistered() && !client->isWelcomeSent()) {   // If fully registered and welcome not sent yet
        client->setWelcomeSent(true);                           // Mark welcome as sent
        sendWelcome(client);                                    // Send welcome messages
    }

}

void CommandHandler::handleJOIN(Client* client, std::istringstream& iss)
{
    std::string channelName;
    std::string key;
    iss >> channelName >> key;
    //validation for the channel name
    if (channelName.empty()) {
        sendError(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
        return;
    }
    if (channelName.length() < 2 || channelName.length() > 50 ||
        (channelName[0] != '#' && channelName[0] != '&')) {
        sendError(client, ERR_BADCHANMASK, channelName + " :Bad Channel Mask");
        return;
    }
    for (size_t i = 0; i < channelName.length(); i++) {
        if (channelName[i] == ' ' || channelName[i] == ',' ||
            channelName[i] == '\0' || channelName[i] == '\r' || channelName[i] == '\n') {
            sendError(client, ERR_BADCHANMASK, channelName + " :Bad Channel Mask");
            return;
        }
    } //end of the validation for the name 

    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        channel = _server->createChannel(channelName, client);
    } else {
        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            sendError(client, ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)");
            return;
        }

        if (!channel->getKey().empty() && channel->getKey() != key) {
            sendError(client, ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)");
            return;
        }

        if (channel->getUserLimit() > 0 &&
            (int)channel->getMembers().size() >= channel->getUserLimit()) {
            sendError(client, ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)");
            return;
        }

        channel->addMember(client);
    }

    std::string joinMsg = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv JOIN " + channelName + "\r\n";
    channel->broadcast(joinMsg);

    if (channel->getTopic().empty())
        sendResponse(client, ":ircserv " + std::string(RPL_NOTOPIC) + " " +  client->getNick() + " " + channelName + " :No topic is set");
    else
        sendResponse(client, ":ircserv " + std::string(RPL_TOPIC) + " " + client->getNick() + " " + channelName + " :" + channel->getTopic());

    std::string namesList = "";
    std::vector<Client*>& members = channel->getMembers();

    for (size_t i = 0; i < members.size(); i++) {
        if (channel->isAdmin(members[i]))
            namesList += "@";
        namesList += members[i]->getNick();
        if (i + 1 < members.size())
            namesList += " ";
    }
    sendResponse(client, ":ircserv " + std::string(RPL_NAMREPLY) + " " +
                 client->getNick() + " = " + channelName + " :" + namesList);

    sendResponse(client, ":ircserv " + std::string(RPL_ENDOFNAMES) + " " +
                 client->getNick() + " " + channelName + " :End of /NAMES list");

    std::cout << "Client " << client->getNick()
              << " joined channel: " << channelName << std::endl;
}
void CommandHandler::handlePRIVMSG(Client* client, std::istringstream& iss) // Handle PRIVMSG command to send a message
{
    std::string targetName;
    iss >> targetName;                                              // Extract target (nick or channel)
    std::string message;
    std::getline(iss, message);                                     // Get the rest as message content
    if (!message.empty() && message[0] == ' ')                      // Remove leading space if present
        message.erase(0, 1);

    if (targetName.empty()) {                                       // If no target provided
        sendError(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
        return;
    }

    Client* target = _server->getClientByNick(targetName);          // Find target client by nick
    if (!target) {                                                   // If target not found
        sendError(client, ERR_NOSUCHNICK, targetName + " :No such nick");
        return;
    }

    std::string prefix = ":" + client->getNick() + " PRIVMSG " + targetName + " :" + message + "\r\n";
    target->sendMessage(prefix);                                    // Send message to target
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

void CommandHandler::handleQUIT(Client* client, std::istringstream& iss) // Handle QUIT command to disconnect client
{
    std::string reason;
    std::getline(iss, reason);                                  // Get quit reason
    if (!reason.empty() && reason[0] == ' ')                    // Remove leading space
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')                    // Remove leading colon
        reason.erase(0, 1);

    std::cout << "Client " << client->getNick() << " quit: " << reason << std::endl; // Debug

    sendResponse(client, "ERROR :Closing connection");          // Notify client
    _server->removeClient(client->getFd());                     // Remove and close fd
}

void CommandHandler::handleMODE(Client* client, std::istringstream& iss) // Handle MODE command to change channel modes
{
    std::string target;
    std::string modeStr;
    iss >> target >> modeStr;                                           // Extract target channel and mode string

    if (target.empty()) {                                               // If no target provided
        sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
        return;
    }

    Channel* channel = _server->getChannel(target);                     // Find channel by name
    if (!channel) {                                                     // If channel not found
        sendError(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
        return;
    }

    if (!channel->isAdmin(client)) {                                    // If client is not operator
        sendError(client, ERR_CHANOPRIVSNEEDED, target + " :You're not channel operator");
        return;
    }

    if (modeStr.empty())                                                // If no mode string, just return
        return;

    std::vector<ModeChange> changes = parseModeString(modeStr, iss);   // Parse mode string into changes

    for (size_t i = 0; i < changes.size(); i++)
    {
        ModeChange& m = changes[i];

        if (m.mode == 'i')                                              // Invite-only mode
            channel->setInviteOnly(m.sign == '+');                      // + = on, - = off

        else if (m.mode == 't')                                         // Topic restriction mode
            channel->setTopicRestricted(m.sign == '+');                 // + = only ops can change topic

        else if (m.mode == 'k')                                         // Channel password mode
        {
            if (m.sign == '+')
                channel->setKey(m.param);                               // Set password
            else
                channel->setKey("");                                    // Remove password
        }

        else if (m.mode == 'o')                                         // Operator privilege mode
        {
            Client* target = _server->getClientByNick(m.param);         // Find target client by nick
            if (!target) {                                              // If target not found
                sendError(client, ERR_NOSUCHNICK, m.param + " :No such nick");
                continue;
            }
            if (!channel->isMember(target)) {                           // If target not in channel
                sendError(client, ERR_USERNOTINCHANNEL, m.param + " " + channel->getName() + " :not in channel");
                continue;
            }
            if (m.sign == '+')
                channel->addAdmin(target);                              // Give operator privilege
            else
                channel->removeAdmin(target);                           // Remove operator privilege
        }

        else if (m.mode == 'l')                                         // User limit mode
        {
            if (m.sign == '+')
                channel->setUserLimit(std::atoi(m.param.c_str()));      // Set user limit
            else
                channel->setUserLimit(0);                               // Remove user limit (0 = no limit)
        }
    }
}
