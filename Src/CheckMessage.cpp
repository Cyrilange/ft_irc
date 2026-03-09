#include "../Inc/CheckMessage.hpp"

CheckMessage::CheckMessage(void) {}
bool CheckMessage::isComandValid(const std::string& command) {
    static std::set<std::string> validCommands;
    
    if (validCommands.empty()) {
        validCommands.insert("PASS");   validCommands.insert("NICK");
        validCommands.insert("USER");   validCommands.insert("JOIN");
        validCommands.insert("PART");   validCommands.insert("PRIVMSG");
        validCommands.insert("NOTICE"); validCommands.insert("KICK");
        validCommands.insert("INVITE"); validCommands.insert("TOPIC");
        validCommands.insert("MODE");   validCommands.insert("QUIT");
        validCommands.insert("PING");   validCommands.insert("PONG");
        validCommands.insert("CAP");
    }
    
    return validCommands.count(command) > 0;
}
CheckMessage::~CheckMessage(void) {}