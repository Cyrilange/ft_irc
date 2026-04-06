#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <poll.h>
#include <sstream>
#include "Client.hpp"
#include <fcntl.h>  // For fcntl() and O_NONBLOCK
#include <signal.h> //for signal like crtl + C
#include <cstdlib>  // For exit()
#include <map>
#include "Channel.hpp"

class Client;
class CommandHandler;
class Channel;

class Server
{
private:

    int _port, _serverSocket;
    std::string _password;
    std::vector<pollfd> _pollfds;
    std::vector<Client*> _clients; // list of connected clients
    CommandHandler* _cmdHandler; // command router 
    static Server*  _instance;
    static volatile sig_atomic_t _running;
    std::map<std::string, Channel*> _channels; 

public:

    Server(int port, const std::string &password);
    ~Server();
    const std::string& getPassword() const;
    void                initSocket();
    void                run();
    void                acceptClient();
    void                receiveMessage(int fd);
    Client*             getClientByFd(int fd);
    Client*             getClientByNick(const std::string& nick);
    std::vector<Client*>& getClients();
    void                removeClient(int fd);
    static void         signalHandler(int sig);
    Channel*            getChannel(const std::string& name);
    std::map<std::string, Channel*>& getChannels();
    Channel*            createChannel(const std::string& name, Client* creator);
    void                removeChannel(const std::string& name);
    Channel*            getChannelByMember(Client* client);

};

#endif