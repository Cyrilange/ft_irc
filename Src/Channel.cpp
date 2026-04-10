
#include "../Inc/Channel.hpp"
#include "../Inc/Client.hpp"  


Channel::Channel(const std::string& name) :
    _name(name),
    _topic(""),
    _key(""),
    _userLimit(0),
    _inviteOnly(false),
    _topicRestricted(false)
{}
Channel::~Channel() {}

// getters
const std::string&    Channel::getName() const { return _name; }
const std::string&    Channel::getTopic() const { return _topic; }
const std::string&    Channel::getKey() const { return _key; }
int                   Channel::getUserLimit() const { return _userLimit; }
bool                  Channel::isInviteOnly() const { return _inviteOnly; }
bool                  Channel::isTopicRestricted() const { return _topicRestricted; }
std::vector<Client*>& Channel::getMembers() { return _members; }
//setters
void    Channel::setTopic(const std::string& topic) { _topic = topic; }
void    Channel::setKey(const std::string& key) { _key = key; }
void    Channel::setUserLimit(int limit) { _userLimit = limit; }
void    Channel::setInviteOnly(bool value) { _inviteOnly = value; }
void    Channel::setTopicRestricted(bool value) { _topicRestricted = value; }
// Member 
void    Channel::addMember(Client* client)                      // Add client to channel
{
    if (!isMember(client))
        _members.push_back(client);
}

void    Channel::removeMember(Client* client)                   // Remove client from channel
{
    for (size_t i = 0; i < _members.size(); i++) {
        if (_members[i] == client) {
            _members.erase(_members.begin() + i);
            return;
        }
    }
}

bool    Channel::isMember(Client* client) const                 // Check if client is in channel
{
    for (size_t i = 0; i < _members.size(); i++) {
        if (_members[i] == client)
            return true;
    }
    return false;
}

// Admin management (IRC operator = mode o)
void    Channel::addAdmin(Client* client)                       // Give operator privilege
{
    if (!isAdmin(client))
        _admins.push_back(client);
}

void    Channel::removeAdmin(Client* client)                    // Remove operator privilege
{
    for (size_t i = 0; i < _admins.size(); i++) {
        if (_admins[i] == client) {
            _admins.erase(_admins.begin() + i);
            return;
        }
    }
}

bool    Channel::isAdmin(Client* client) const                  // Check if client is operator
{
    for (size_t i = 0; i < _admins.size(); i++) {
        if (_admins[i] == client)
            return true;
    }
    return false;
}

// Invite management
void    Channel::addInvited(Client* client)                     // Add client to invite list
{
    if (!isInvited(client))
        _invited.push_back(client);
}

bool    Channel::isInvited(Client* client) const                // Check if client is invited
{
    for (size_t i = 0; i < _invited.size(); i++) {
        if (_invited[i] == client)
            return true;
    }
    return false;
}

void    Channel::broadcast(const std::string& msg, Client* exclude) // Send message to all members
{
    for (size_t i = 0; i < _members.size(); i++) {
        if (_members[i] != exclude)                             // Skip excluded client
            _members[i]->sendMessage(msg);
    }
}