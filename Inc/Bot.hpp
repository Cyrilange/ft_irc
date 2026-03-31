#pragma once


#include <string>
#include <ctime>
#include <vector>




class Client; 
class Channel;

class Bot {
public:
    Bot();
    ~Bot();
    std::string getTime();
    std::string handleMessage(const std::string& msg, const std::string& nick, Channel* channel);
};