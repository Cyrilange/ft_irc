#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <sys/socket.h>

class Client;

struct ModeChange {
    char        sign;    // '+' or '-'
    char        mode;    // 'i', 't', 'k', 'o', 'l'
    std::string param;   // parameter
};

class Channel
{
private:
    std::string          _name;             // channel name 
    std::string          _topic;            // channel topic
    std::string          _key;              // password (mode k)
    int                  _userLimit;        // max users (mode l), 0 = no limit
    bool                 _inviteOnly;       // mode i
    bool                 _topicRestricted;  // mode t
    std::vector<Client*> _members;          // menbers of the the channel
    std::vector<Client*> _admins;        	// clients with operator privilege, operator is admin ( in irc we say operator) 
    std::vector<Client*> _invited;          // clients invited (mode i)
   

public:
    Channel(const std::string& name);
    ~Channel();

    // getters
    const std::string&    getName() const;
    const std::string&    getTopic() const;
    const std::string&    getKey() const;
    int                   getUserLimit() const;
    bool                  isInviteOnly() const;
    bool                  isTopicRestricted() const;
    std::vector<Client*>& getMembers();
	//setters
    void    setTopic(const std::string& topic);
    void    setKey(const std::string& key);
    void    setUserLimit(int limit);
    void    setInviteOnly(bool value);
    void    setTopicRestricted(bool value);
    // Member 
    void    addMember(Client* client);
    void    removeMember(Client* client);
    bool    isMember(Client* client) const;
    // Operator , I wrote admin to be reader friendly but the IRC term is operator so it is the mode o 
    void    addAdmin(Client* client);
    void    removeAdmin(Client* client);
    bool    isAdmin(Client* client) const;
    // Invite 
    void    addInvited(Client* client);
    bool    isInvited(Client* client) const;
    // For all menbers, messages
    void    broadcast(const std::string& msg, Client* exclude = NULL);
};

#endif