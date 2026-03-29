#include "../Inc/Server.hpp"
#include <iostream>
#include <cstdlib>
#include "../Inc/Channel.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) { std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;  return 1;}
    srand(time(NULL));
	int port = std::atoi(argv[1]);
    std::string password = argv[2];
	if (port < 1024 || port > 65535) {return (std::cerr << "Error: invalid port\n", 1);}
    try
    {
        Server server(port, password);
        server.run();
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}