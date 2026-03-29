#pragma once


#include <string>
#include <ctime>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"

class Bot {
public:
    Bot();
    ~Bot();
    std::string getTime();
    std::string handleMessage(const std::string& msg, const std::string& nick, Channel* channel);
};