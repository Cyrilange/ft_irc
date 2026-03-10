#include "../Inc/Server.hpp"      // Include the header file for the Server class
#include "../Inc/CommandHandler.hpp"
#include <iostream>               // For std::cout and std::cerr
#include <cstring>                // For memset
#include <sys/socket.h>           // For socket functions (socket, bind, listen, accept)
#include <netinet/in.h>           // For sockaddr_in structure
#include <unistd.h>               // For close()
#include <arpa/inet.h>            // For htons and network utilities


// Constructor of the Server class
Server::Server(int port, const std::string &password) : _port(port), _password(password) { _cmdHandler = new CommandHandler(this);initSocket();}
const std::string& Server::getPassword() const { return _password; }
std::vector<Client*>& Server::getClients() {return this->_clients;}
// Accept a new client connection
void Server::acceptClient()
{
    int clientFd = accept(_serverSocket, NULL, NULL);

    if (clientFd < 0)
    {
        std::cerr << "accept failed" << std::endl;
        return;
    }

    Client* newClient = new Client(clientFd);
    _clients.push_back(newClient);

    pollfd pfd;
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    _pollfds.push_back(pfd);
}

Client* Server::getClientByFd(int fd)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->getFd() == fd)
            return _clients[i];
    }
    return NULL; // pas trouvé
}

void Server::removeClient(int fd)
{
    // fermer le socket
    close(fd);

    // supprimer le client du vector _clients
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->getFd() == fd)
        {
            delete _clients[i];            // libérer la mémoire
            _clients.erase(_clients.begin() + i);
            break;
        }
    }

    // supprimer le client du vector _pollfds
    for (size_t i = 0; i < _pollfds.size(); ++i)
    {
        if (_pollfds[i].fd == fd)
        {
            _pollfds.erase(_pollfds.begin() + i);
            break;
        }
    }
}

// Function responsible for creating and configuring the server socket
// Function to initialize the server socket and start listening for connections
void Server::initSocket()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP IPv4 socket

    if (_serverSocket < 0)                            // If socket creation failed
        throw std::runtime_error("socket() failed");  // Throw an exception

    int opt = 1;                                       // Option value for setsockopt

    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() failed"); // Allow port reuse after restart

    sockaddr_in addr;                                  // Structure for server address

    std::memset(&addr, 0, sizeof(addr));                // Clear structure memory

    addr.sin_family = AF_INET;                          // Use IP addressing
    addr.sin_port = htons(_port);                       // Convert port to network byte order
    addr.sin_addr.s_addr = INADDR_ANY;                  // Accept connections from any IP

    if (bind(_serverSocket, (sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");      // Bind socket to address and port

    if (listen(_serverSocket, SOMAXCONN) < 0)
        throw std::runtime_error("listen() failed");     // Start listening for connections

    pollfd serverPoll;                                   // Structure used by poll()

    serverPoll.fd = _serverSocket;                       // File descriptor to monitor
    serverPoll.events = POLLIN;                          // Detect data available to read
    serverPoll.revents = 0;                               // Initialize event flags to zero

    _pollfds.push_back(serverPoll);                       // Add server socket to poll list

    std::cout << "Server started on port " << _port << std::endl; // Print success message
}

// Function used to accept a new client connection
void Server::receiveMessage(int fd)
{
    char buffer[1024];
    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0)
    {
        std::cout << "Client disconnected fd=" << fd << std::endl;
        close(fd);
        removeClient(fd);
        return;
    }

    buffer[bytes] = '\0';

    Client* client = getClientByFd(fd); // helper function to find client*
    if (!client)
        return;

    client->appendToBuffer(std::string(buffer));

    std::string msg;
    while (!(msg = client->extractMessage()).empty())
	{
		// std::cout << "debug : Received : " << msg << std::endl;
    	_cmdHandler->handleCommand(client, msg);
	}
}


//function to receive message 


// Main server loop
	void Server::run()
	{
		while (true)
		{
			int ret = poll(&_pollfds[0], _pollfds.size(), -1);

			if (ret < 0)
			{
				std::cerr << "poll error" << std::endl;
				return;
			}

			for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].revents == 0) // no event, skip safely
			continue;

		if (_pollfds[i].revents & POLLIN) // data available
		{
			if (_pollfds[i].fd == _serverSocket) // new client
			{
				acceptClient();
			}
			else // message from client
			{
				receiveMessage(_pollfds[i].fd);
			}
		}
		else if (_pollfds[i].revents & POLLHUP) // client closed connection
		{
			close(_pollfds[i].fd);
			_pollfds.erase(_pollfds.begin() + i);
			--i;
		}
		else if (_pollfds[i].revents & POLLERR) // error on socket
		{
			close(_pollfds[i].fd);
			_pollfds.erase(_pollfds.begin() + i);
			--i;
		}
		else if (_pollfds[i].revents & POLLNVAL) // invalid fd
		{
			_pollfds.erase(_pollfds.begin() + i);
			--i;
		}
	}
    }
}

// Destructor of the Server class
Server::~Server()
{
    // Close all file descriptors stored in the poll list
    for (size_t i = 0; i < _pollfds.size(); i++) {
        close(_pollfds[i].fd);
	}
	delete _cmdHandler;
}



//for now open a terminal and write . ./ircserv 6667 pass, open an other terminal and write  nc localhost 6667
//the first terminal should have new clients written


/*
** Server.cpp
**
** WHAT IS DONE:
** - TCP socket creation with SO_REUSEADDR
** - bind() and listen() on given port
** - poll() event loop for non-blocking I/O given by the subject avec fcnl 
** - acceptClient() for new incoming connections
** - receiveMessage() with buffer and message extraction
** - removeClient() cleans both _clients and _pollfds
** - POLLHUP, POLLERR, POLLNVAL handled in event loop
** - CommandHandler created and deleted properly
**
** WHAT IS MISSING:
** - getClientByNick() : needed for PRIVMSG and KICK to find a client by nickname
** - getChannel()      : needed once Channel class exists, to find a channel by name
** - addChannel()      : create and store a new channel
** - removeChannel()   : delete a channel when last member leaves
** - broadcast()       : send a message to all connected clients (needed for QUIT)
** - Signal handling   : CTRL+C (SIGINT) should cleanly close all fds and exit
** - Non-blocking fds  : fcntl() should be set on accepted client sockets
** - removeClient()    : should also remove client from all channels he was in
** - Constructor       : initSocket() should be called before _cmdHandler is created
*/