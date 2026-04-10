
#include "../Inc/Client.hpp"


Client::Client(int fd) : _fd(fd) , _passAccepted(false) , _welcomeSent(false), _hasPing(true) {}
Client::~Client() {}
int Client::getFd() const { return _fd; }
void Client::appendToBuffer(const std::string &data) { _buffer += data; }

std::string Client::extractMessage()
{
    size_t pos = _buffer.find("\r\n");

    if (pos == std::string::npos)
        pos = _buffer.find('\n');

    if (pos == std::string::npos)
        return "";

    std::string message = _buffer.substr(0, pos);

    if (_buffer[pos] == '\r')
        _buffer.erase(0, pos + 2);
    else
        _buffer.erase(0, pos + 1);

    return message;
}

void Client::setNick(const std::string &nick) { _nick = nick; }
std::string Client::getNick() const { return _nick; }
void Client::setUsername(const std::string &username) { _username = username; }
void Client::setRealname(const std::string &realname) {_realname = realname;}
std::string Client::getUsername() const { return _username; }
void Client::setPassAccepted(bool value) { _passAccepted = value; }
bool Client::isPassAccepted() const { return _passAccepted;}
bool Client::isRegistered() const { return _passAccepted && !_nick.empty() && !_username.empty() && _hasPing == true;}
void Client::sendMessage(const std::string& msg)
{
    std::string formatted = msg;

    if (formatted.length() < 2 || formatted.substr(formatted.length() - 2) != "\r\n")
        formatted += "\r\n";

    send(_fd, formatted.c_str(), formatted.size(), 0);
}
bool Client::isWelcomeSent() const { return _welcomeSent; }
void Client::setWelcomeSent(bool v) { _welcomeSent = v; }

