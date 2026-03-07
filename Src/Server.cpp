#include "../Inc/Server.hpp"      // Include the header file for the Server class

#include <iostream>               // For std::cout and std::cerr
#include <cstring>                // For memset
#include <sys/socket.h>           // For socket functions (socket, bind, listen, accept)
#include <netinet/in.h>           // For sockaddr_in structure
#include <unistd.h>               // For close()
#include <arpa/inet.h>            // For htons and network utilities

// Constructor of the Server class
Server::Server(int port, const std::string &password)
{
    _port = port;                 // Store the port number provided by the user
    _password = password;         // Store the connection password

    initSocket();                 // Initialize the server socket and start listening
}

// Function responsible for creating and configuring the server socket
void Server::initSocket()
{
    // Create a TCP socket using IPv4
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // If socket creation failed, throw an exception
    if (_serverSocket < 0)
        throw std::runtime_error("socket() failed");

    // Optional: make the socket non-blocking (commented for now)
	//fcntl(_serverSocket, F_SETFL, O_NONBLOCK);

    int opt = 1; // Option value used for setsockopt

    // Allow the server to reuse the same port immediately after restart
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() failed");

    sockaddr_in addr; // Structure that will hold the server address information

    // Set all bytes of the address structure to zero
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;            // Specify IPv4 addressing
    addr.sin_port = htons(_port);         // Convert port to network byte order
    addr.sin_addr.s_addr = INADDR_ANY;    // Accept connections from any IP address

    // Bind the socket to the specified IP and port
    if (bind(_serverSocket, (sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    // Start listening for incoming client connections
    if (listen(_serverSocket, SOMAXCONN) < 0)
        throw std::runtime_error("listen() failed");

    pollfd serverPoll;                    // Structure used by poll() to monitor file descriptors

    serverPoll.fd = _serverSocket;        // The file descriptor to monitor (the server socket)
    serverPoll.events = POLLIN;           // We want to detect when data is available to read

    // Add the server socket to the poll file descriptor list
    _pollfds.push_back(serverPoll);

    // Print a message indicating that the server started correctly
    std::cout << "Server started on port " << _port << std::endl;
}

// Function used to accept a new client connection
void Server::acceptClient()
{
    // Accept a new client connection from the server socket
    int clientFd = accept(_serverSocket, NULL, NULL);

    // If accept fails, print an error and stop
    if (clientFd < 0)
    {
        std::cerr << "accept() failed" << std::endl;
        return;
    }

    pollfd clientPoll;           // Create a poll structure for the new client

    clientPoll.fd = clientFd;    // Store the client's file descriptor
    clientPoll.events = POLLIN;  // Monitor the client for incoming data

    // Add the new client to the poll list so we can monitor it
    _pollfds.push_back(clientPoll);

    // Print the new client's file descriptor for debugging
    std::cout << "New client connected fd=" << clientFd << std::endl;
}

// Main server loop
void Server::run()
{
    while (true) // Infinite loop to keep the server running
    {
        // poll() waits for activity on the file descriptors
        int ret = poll(&_pollfds[0], _pollfds.size(), -1);

        // If poll fails, print an error and stop the server
        if (ret < 0)
        {
            std::cerr << "poll error" << std::endl;
            return;
        }

        // Loop through all monitored file descriptors
        for (size_t i = 0; i < _pollfds.size(); i++)
        {
            // Check if there is data to read on this descriptor
            if (_pollfds[i].revents & POLLIN)
            {
                // If the activity is on the server socket
                if (_pollfds[i].fd == _serverSocket)

                    // That means a new client wants to connect
                    acceptClient();
            }
        }
    }
}

// Destructor of the Server class
Server::~Server()
{
    // Close all file descriptors stored in the poll list
    for (size_t i = 0; i < _pollfds.size(); i++)
        close(_pollfds[i].fd);
}



//for now open a terminal and write . ./ircserv 6667 pass, open an other terminal and write  nc localhost 6667
//the first terminal should have new clients written