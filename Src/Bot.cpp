#include "../Inc/Bot.hpp"
#include "../Inc/Client.hpp"
#include "../Inc/Channel.hpp"


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
    std::vector<std::string> quotes;
    quotes.push_back("Discipline is choosing between what you want now and what you want most.");
    quotes.push_back("You don't rise to the level of your goals, you fall to the level of your systems.");
    quotes.push_back("Hard choices, easy life. Easy choices, hard life.");
    quotes.push_back("Success is not final, failure is not fatal: it is the courage to continue that counts.");
    quotes.push_back("The man who moves a mountain begins by carrying away small stones.");
    std::string quote = quotes[rand() % quotes.size()];

    std::vector<std::string> jokes;
    jokes.push_back("Why do programmers prefer dark mode? Because light attracts bugs.");
    jokes.push_back("There are only 10 types of people in the world: those who understand binary and those who don't.");
    jokes.push_back("Why do Java developers wear glasses? Because they don't C.");
    jokes.push_back("A programmer's wife tells him: 'Go to the store and buy a loaf of bread. If they have eggs, buy a dozen.' He comes back with 12 loaves of bread.");
    jokes.push_back("I just got fired from the keyboard factory. They said I wasn't putting in enough shifts.");
    std::string joke = jokes[rand() % jokes.size()];

    std::vector<std::string> hi;
    hi.push_back("Hi there.");
    hi.push_back("Hey lovely.");
    hi.push_back("Good day sweety.");
    std::string hello = hi[rand() % hi.size()];

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
        if (!channel)
            return ":ircbot!bot@ircserv PRIVMSG " + nick + " :You are not in a channel.";
        const std::vector<Client*>& members = channel->getMembers();
        std::string res = ":ircbot!bot@ircserv PRIVMSG " + nick + " :Users: ";
        for (size_t i = 0; i < members.size(); i++) {
            res += members[i]->getNick();
            if (i != members.size() - 1)
                res += ", ";
        }
        return res;
    }
    if (msg == "!channel") {
        if (!channel)
            return ":ircbot!bot@ircserv PRIVMSG " + nick + " :You are not in a channel.";
        return ":ircbot!bot@ircserv PRIVMSG " + nick + " :You are on channel " + channel->getName();
    }
    return "";
}

Bot::~Bot() {

}