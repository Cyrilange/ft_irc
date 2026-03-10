#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <poll.h>
#include <sstream>
#include "Client.hpp"
#include <fcntl.h>  // For fcntl() and O_NONBLOCK

class Client;
class CommandHandler;

class Server
{
private:

    int _port, _serverSocket;
    std::string _password;
    std::vector<pollfd> _pollfds;
    std::vector<Client*> _clients; // list of connected clients
    CommandHandler* _cmdHandler; // command router 

public:

    Server(int port, const std::string &password);
    ~Server();
    const std::string& getPassword() const;
    void initSocket();
    void run();
    void acceptClient();
    void receiveMessage(int fd);
    Client* getClientByFd(int fd);
    std::vector<Client*>& getClients();
    void removeClient(int fd);

};

#endif