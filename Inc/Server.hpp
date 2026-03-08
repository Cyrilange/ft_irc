#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <poll.h>
#include "Client.hpp"

class Client;

class Server
{
private:

    int _port, _serverSocket;
    std::string _password;
    std::vector<pollfd> _pollfds;
    std::vector<Client*> _clients; // list of connected clients

public:

    Server(int port, const std::string &password);
    ~Server();

    void initSocket();
    void run();
    void acceptClient();
    void receiveMessage(int fd);
    Client* getClientByFd(int fd);
    void removeClient(int fd);
};

#endif