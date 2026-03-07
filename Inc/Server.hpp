#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <poll.h>

class Server
{
private:

    int _port, _serverSocket;
    std::string _password;
    std::vector<pollfd> _pollfds;

public:

    Server(int port, const std::string &password);
    ~Server();

    void initSocket();
    void run();
    void acceptClient();
};

#endif