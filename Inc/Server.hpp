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
    static Server*  _instance;
    static bool     _running;

public:

    Server(int port, const std::string &password);
    ~Server();
    const std::string& getPassword() const;
    void initSocket();
    void run();
    void acceptClient();
    void receiveMessage(int fd);
    Client* getClientByFd(int fd);
    Client* getClientByNick(const std::string& nick);
    std::vector<Client*>& getClients();
    void removeClient(int fd);
    static void    signalHandler(int sig); // static signal handler

};

#endif