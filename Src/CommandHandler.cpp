/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csalamit <csalamit@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 23:36:40 by csalamit          #+#    #+#             */
/*   Updated: 2026/04/03 21:39:36 by csalamit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../Inc/CommandHandler.hpp"
class Channel;


CommandHandler::CommandHandler(Server* server) : _server(server) { initHandlers(); } // Constructor: store server pointer and initialize command handlers

CommandHandler::~CommandHandler() {} // Destructor

Bot CommandHandler::getBot() {
    return _bot;
}

//innit for the constructor 
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
    _handlers["PART"] = &CommandHandler::handlePART;     //to quit the chanell but not being disconected for the socket
    //part      
    //kick
    _handlers["TOPIC"] = &CommandHandler::handleTOPIC;
    //invite
    //who
}


/*##################################FUNCTIONS UTILS###########################################################################*/
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

static std::string cleanChannelName(const std::string& name)
{
    std::string res = name;

    if (!res.empty() && res[res.size() - 1] == '\r')
        res.erase(res.size() - 1);

    return res;
}

static bool isValidChannelName(const std::string& name)
{
    if (name.length() < 2 || name.length() > 50)
        return false;

    if (name[0] != '#' && name[0] != '&')
        return false;

    for (size_t i = 0; i < name.length(); i++)
    {
        if (name[i] == ' ' || name[i] == ',' ||
            name[i] == '\r' || name[i] == '\n' ||
            name[i] == '\0')
            return false;
    }
    return true;
}

static std::string buildNamesList(Channel* channel)
{
    std::string list;
    std::vector<Client*>& members = channel->getMembers();

    for (size_t i = 0; i < members.size(); i++)
    {
        if (channel->isAdmin(members[i]))
            list += "@";

        list += members[i]->getNick();

        if (i + 1 < members.size())
            list += " ";
    }
    return list;
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

/*##################################FUNCTIONS UTILS END###########################################################################*/

//PING
void CommandHandler::handlePING(Client* client, std::istringstream& iss) // Handle PING to keep connection alive
{
    std::string token;
    iss >> token;                                               // Extract ping token sent by client

    if (token.empty())                                          // If no token provided
        sendResponse(client, ":ircserv PONG ircserv");          // Send basic pong
    else
        sendResponse(client, ":ircserv PONG ircserv :" + token); // Echo token back to client
}

//CAP
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
    for (size_t i = 0; i < cmd.length(); ++i) {
        cmd[i] = std::toupper(static_cast<unsigned char>(cmd[i]));// because IRC is not sensible so COMMAND or command or even coMand shoudl work
    }
    if (!client->isPassAccepted() && cmd != "PASS" && cmd != "CAP" && cmd != "NICK" && cmd != "USER" && cmd != "PING")
    {
        sendResponse(client, "ERROR :You need to authenticate with PASS first"); // Reject unauthenticated command
        return;
    }
    std::map<std::string, HandlerFunc>::iterator it = _handlers.find(cmd);

    if (it == _handlers.end()) {
        std::cout << "Unknown command: " << cmd << std::endl; // if you do like NICKnickname with no space bwteen the command and the name youll have an error on the server side 
        sendError(client, ERR_UNKNOWNCOMMAND, cmd + " :Unknown command");
        return;
    }   
    (this->*(it->second))(client, iss);                                 // Call the corresponding handler     
}

//WELCOME
void CommandHandler::sendWelcome(Client* client)
{
    const std::string& nick = client->getNick();

    sendResponse(client, ":ircserv " + std::string(RPL_WELCOME)  + " " + nick + " :Welcome to the ircserv Network, " + nick);
    sendResponse(client, ":ircserv " + std::string(RPL_YOURHOST) + " " + nick + " :Your host is ircserv, running version 1.0");
    sendResponse(client, ":ircserv " + std::string(RPL_CREATED)  + " " + nick + " :This server was created " + _bot.getTime());
    sendResponse(client, ":ircserv " + std::string(RPL_MYINFO)   + " " + nick + " ircserv 1.0 o itklo");

    sendResponse(client, ":ircbot!bot@ircserv PRIVMSG " + nick + " :Welcome " + nick + "! I am ircbot, type !help for help.");
}

//PASSWORD
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



void CommandHandler::handleNICK(Client* client, std::istringstream& iss)
{
    std::string nick;
    iss >> nick;

    if (nick.empty()) {
        sendError(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
        return;
    }
    if (nick[nick.size() - 1] == '\r') { nick.erase(nick.size() - 1); }
    if (!isValidNick(nick)) {
        sendError(client, ERR_ERRONEUSNICK, nick + " :Erroneous nickname");
        return;
    }

    std::vector<Client*>& clients = _server->getClients();
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i]->getNick() == nick) {
            sendError(client, ERR_NICKNAMEINUSE, nick + " :Nickname is already in use");
            return;
        }
    }

    std::string oldNick = client->getNick(); // save nick old
    client->setNick(nick);

    if (client->isRegistered() && !client->isWelcomeSent()) {
        client->setWelcomeSent(true);
        sendWelcome(client);
        return;
    }
    if (client->isWelcomeSent() && !oldNick.empty()) {
        std::string nickMsg = ":" + oldNick + "!" + client->getUsername() + "@ircserv NICK " + nick;
        std::map<std::string, Channel*>& channels = _server->getChannels();
        for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if (it->second->isMember(client)) 
                it->second->broadcast(nickMsg, client); //broadcast but excluse client
              
        }
    sendResponse(client, nickMsg);
    }
}

void CommandHandler::handleUSER(Client* client, std::istringstream& iss) 
{
    std::string username, hostname, servername, realname;
    // extract the args
    iss >> username >> hostname >> servername;
    std::getline(iss, realname);
    //for RCF 2812 we need 4 args for user <username> <hostname> <servername> :<realname> (user ali 0 * :My realname is)
    if (username.empty() || realname.empty()) {
        sendError(client, "461", "USER :Not enough parameters");
        return;
    }
    if (!client->getUsername().empty()) {
        sendError(client, "462", ":You may not reregister");
        return;
    }
    if (!realname.empty() && realname[0] == ' ') realname.erase(0, 1);
    if(realname[0] != ':') {sendError(client, "461", "USER :Not enough parameters");
        return;}
    if (!realname.empty() && realname[0] == ':') realname.erase(0, 1);
    client->setUsername(username);
    client->setRealname(realname);
    if (!client->getNick().empty() && !client->isWelcomeSent()) {
        client->setWelcomeSent(true);
        sendWelcome(client);
    }
}

//JOIN 

void CommandHandler::handleJOIN(Client* client, std::istringstream& iss)
{
    std::string channelName, key;
    iss >> channelName >> key; channelName = cleanChannelName(channelName);
    if (channelName.empty()) {
        sendError(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
        return;
    }

    if (!isValidChannelName(channelName)) {
        sendError(client, ERR_BADCHANMASK, channelName + " :Bad Channel Mask");
        return;
    }
    Channel* channel = _server->getChannel(channelName);
    if (!channel) { channel = _server->createChannel(channelName, client);}
    else {
        if (channel->isMember(client)) {return;}
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
    std::string joinMsg = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv JOIN " + channelName;
    channel->broadcast(joinMsg, client);
    sendResponse(client, joinMsg);
    if (channel->getTopic().empty())
        sendResponse(client, ":ircserv " + std::string(RPL_NOTOPIC) + " " + client->getNick() + " " + channelName + " :No topic is set");
    else
        sendResponse(client, ":ircserv " + std::string(RPL_TOPIC) + " " + client->getNick() + " " + channelName + " :" + channel->getTopic());
    std::string namesList = buildNamesList(channel);
    sendResponse(client, ":ircserv " + std::string(RPL_NAMREPLY) + " " + client->getNick() + " = " + channelName + " :" + namesList);
    sendResponse(client, ":ircserv " + std::string(RPL_ENDOFNAMES) + " " +  client->getNick() + " " + channelName + " :End of /NAMES list");
}

//TOPIC

void CommandHandler::handleTOPIC(Client* client, std::istringstream& iss)
{
    std::string targetName;
    iss >> targetName;

    std::string message;
    std::getline(iss, message);

    if (!message.empty() && message[0] == ' ') { message.erase(0, 1); }
    if (!message.empty() && message[0] == ':') { message.erase(0, 1); }

    if (targetName.empty()) {
        sendError(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
        return;
    }

    Channel* channel = _server->getChannel(targetName);
    if (!channel) {
        sendError(client, ERR_NOSUCHCHANNEL, targetName + " :No such channel");
        return;
    }

    if (!channel->isMember(client)) {
        sendError(client, ERR_NOTONCHANNEL, targetName + " :You're not on that channel");
        return;
    }

    if (message.empty()) {
        std::string topic = channel->getTopic();
        if (topic.empty())
            sendResponse(client, ":ircserv " + std::string(RPL_NOTOPIC) + " " + client->getNick() + " " + targetName + " :No topic is set");
        else
            sendResponse(client, ":ircserv " + std::string(RPL_TOPIC) + " " + client->getNick() + " " + targetName + " :" + topic);
        return;
    }
    if (channel->isTopicRestricted() && !channel->isAdmin(client)) {
        sendError(client, ERR_CHANOPRIVSNEEDED, targetName + " :You're not channel operator");
        return;
    }
    channel->setTopic(message);
    std::string prefix = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv";
    channel->broadcast(prefix + " TOPIC " + targetName + " :" + message, NULL);
}
//PRIVMSG


void CommandHandler::handlePRIVMSG(Client* client, std::istringstream& iss)
{
    std::string targetName;
    iss >> targetName;

    std::string message;
    std::getline(iss, message);

    if (!message.empty() && message[0] == ' ') { message.erase(0, 1); }
    if (!message.empty() && message[0] == ':') { message.erase(0, 1); }

    if (targetName.empty() || message.empty()) {
        sendError(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
        return;
    }

    if (targetName == "ircbot") {
        Channel* botChannel = _server->getChannelByMember(client);
        std::string response = _bot.handleMessage(message, client->getNick(), botChannel);
        if (!response.empty())
            sendResponse(client, response);
        return;
    }
    std::string prefix = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv PRIVMSG " + targetName + " :" + message;
    Channel* channel = NULL;
    if (targetName[0] == '#' ||targetName[0] == '&' ) {
        channel = _server->getChannel(targetName);
        if (!channel) {
            sendError(client, ERR_NOSUCHCHANNEL, targetName + " :No such channel");
            return;
        }
        if (!channel->isMember(client)) {
            sendError(client, ERR_CANNOTSENDTOCHAN, targetName + " :Cannot send to channel");
            return;
        }
        channel->broadcast(prefix, client);
    }
    else {
        Client* target = _server->getClientByNick(targetName);
        if (!target) {
            sendError(client, ERR_NOSUCHNICK, targetName + " :No such nick");
            return;
        }
        target->sendMessage(prefix + "\r\n");
    }
}

std::vector<ModeChange> CommandHandler::parseModeString(const std::string& modeStr, std::istringstream& iss) // Parse mode string into list of ModeChange structs
{
    std::vector<ModeChange> changes;
    if (modeStr.empty())   {return changes; }     // If no mode string provided, Return empty list                                          
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

//QUIT
void CommandHandler::handleQUIT(Client* client, std::istringstream& iss)
{
    std::string reason;
    std::getline(iss, reason);

    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);

    std::string quitMsg = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv QUIT :" + reason;
    // remove client from all channels
    std::map<std::string, Channel*>& channels = _server->getChannels();
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel* channel = it->second;

        if (channel->isMember(client)) {
            channel->broadcast(quitMsg, client);
            channel->removeMember(client);
        }
    }
    std::cout << "Client " << client->getNick() << " quit: " << reason << std::endl;
    sendResponse(client, "ERROR :Closing connection");
    _server->removeClient(client->getFd());
}
//PART
void CommandHandler::handlePART(Client* client, std::istringstream& iss)
{
    std::string chanName;
    iss >> chanName;
    if (chanName.empty()) {
        sendError(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
        return;
    }
    Channel* channel = _server->getChannel(chanName);
    if (!channel) {
        sendError(client, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
        return;
    }
    if (!channel->isMember(client)) {
        sendError(client, ERR_NOTONCHANNEL, chanName + " :You're not on that channel");
        return;
    }
    std::string reason;
    std::getline(iss, reason);

    if (!reason.empty() && reason[0] == ' ') {reason.erase(0, 1);}
    if (!reason.empty() && reason[0] == ':')  reason.erase(0, 1);
    std::string partMsg = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv PART " + chanName;
    if (!reason.empty()) {partMsg += " :" + reason;}
    channel->broadcast(partMsg, client);
    sendResponse(client, partMsg);
    channel->removeMember(client);
    if (channel->getMembers().empty()) { _server->removeChannel(chanName); }
}
//mode
void CommandHandler::handleMODE(Client* client, std::istringstream& iss)
{
    std::string target;
    std::string modeStr;
    iss >> target >> modeStr;

    if (target.empty()) {
        sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
        return;
    }
    Channel* channel = _server->getChannel(target);
    if (!channel) {
        sendError(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
        return;
    }
    if (!channel->isAdmin(client)) {
        sendError(client, ERR_CHANOPRIVSNEEDED, target + " :You're not channel operator");
        return;
    }
    if (modeStr.empty()) { return; }

    std::vector<ModeChange> changes = parseModeString(modeStr, iss);
    std::string params = ""; 

    for (size_t i = 0; i < changes.size(); i++)
    {
        ModeChange& m = changes[i];

        if (m.mode == 'i')
            channel->setInviteOnly(m.sign == '+');
        else if (m.mode == 't')
            channel->setTopicRestricted(m.sign == '+');
        else if (m.mode == 'k')
        {
            if (m.sign == '+') {
                channel->setKey(m.param);
                params += " *" + m.param;  // hide password
            } else
                channel->setKey("");
        }
        else if (m.mode == 'o')
        {
            Client* target = _server->getClientByNick(m.param);
            if (!target) {
                sendError(client, ERR_NOSUCHNICK, m.param + " :No such nick");
                continue;
            }
            if (!channel->isMember(target)) {
                sendError(client, ERR_USERNOTINCHANNEL, m.param + " " + channel->getName() + " :not in channel");
                continue;
            }
            if (m.sign == '+')
                channel->addAdmin(target);
            else
                channel->removeAdmin(target);
            params += " " + m.param;  // add nick
        }
        else if (m.mode == 'l')
        {
            if (m.sign == '+') {
                channel->setUserLimit(std::atoi(m.param.c_str()));
                params += " " + m.param;  // add limit
            } else
                channel->setUserLimit(0);
        }
    }
    std::string modeMsg = ":" + client->getNick() + "!" + client->getUsername() + "@ircserv MODE " + target + " " + modeStr + params;
    channel->broadcast(modeMsg, NULL);
}