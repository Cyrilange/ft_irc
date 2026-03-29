#include "../Inc/Bot.hpp"
class Channel;

Bot::Bot() {
	
}

std::string Bot::getTime() {
	std::time_t t = std::time(NULL);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%A, %B %d, %Y %H:%M:%S", std::localtime(&t));
	return std::string(buffer);
}

std::string Bot::handleMessage(const std::string& msg, const std::string& nick, Channel* channel)
{
    std::vector<std::string> quotes = {
        "Discipline is choosing between what you want now and what you want most.",
        "You don’t rise to the level of your goals, you fall to the level of your systems.",
        "Hard choices, easy life. Easy choices, hard life.",
        "Success is not final, failure is not fatal: it is the courage to continue that counts.",
        "The man who moves a mountain begins by carrying away small stones."
    }; int index = rand() % quotes.size(); std::string quote = quotes[index];

    std::vector<std::string> jokes = {
        "Why do programmers prefer dark mode? Because light attracts bugs.",
        "There are only 10 types of people in the world: those who understand binary and those who don’t.",
        "Why do Java developers wear glasses? Because they don’t C.",
        "A programmer’s wife tells him: 'Go to the store and buy a loaf of bread. If they have eggs, buy a dozen.' He comes back with 12 loaves of bread.",
        "I just got fired from the keyboard factory. They said I wasn't putting in enough shifts."
    }; int index_jokes = rand() % jokes.size(); std::string joke = jokes[index_jokes];

    std::vector<std::string> hi = {
        "Hi there.",
        "Hey lovely.",
        "good day sweety."
    }; int index_hi = rand() % hi.size(); std::string hello = hi[index_hi];

    if (msg == "!time")
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :" + getTime();
    if (msg == "!help")
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :Available commands: !time !help !joke !quote !channel !hi !list";
    if (msg == "!quote")
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :" + quote; 
    if (msg == "!joke")
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :" + joke;
    if (msg == "!hi")
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :" + hello;
    if (msg == "!list") {
            const std::vector<Client*>& members = channel->getMembers();
            std::string res = ":ircbot!bot@ircserv PRIVMSG " + nick + " :Users: ";
        
            for (size_t i = 0; i < members.size(); i++) {
                res += members[i]->getNick();
                if (i != members.size() - 1)
                    res += ", ";
            }
            return res;
        }
    if(msg == "!channel")
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :you are on channel " + channel->getName();
    return "";
}

Bot::~Bot() {

}