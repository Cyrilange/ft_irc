FT_IRC - TODO LIST
==================

MANDATORY
---------

CORE FIXES (do these first)
[ ] Fix handleNICK - call setNick() BEFORE sendWelcome()
[ ] Fix handleCommand - allow CAP, NICK, USER before PASS is accepted
[ ] Fix sendMessage in Client - remove CheckMessage validation, just send
[ ] Add PING/PONG handler - HexChat disconnects if no PONG response

CHANNEL CLASS (needed for everything below)
[ ] Create Channel class with: name, topic, key, userLimit, inviteOnly, topicRestricted
[ ] Add members list (vector<Client*>)
[ ] Add operators list (vector<Client*>)
[ ] Add broadcast() method to send message to all members
[ ] Add isOperator() method
[ ] Store channels in Server (map<string, Channel*>)

COMMANDS
[ ] JOIN  - create/join channel, broadcast JOIN, send RPL_NAMREPLY (353), RPL_ENDOFNAMES (366)
[ ] PART  - leave channel, broadcast PART to members
[ ] QUIT  - disconnect client, broadcast QUIT to all channels client was in
[ ] PRIVMSG - route message to channel or nick properly
[ ] KICK  - operator removes a client from channel
[ ] INVITE - operator invites a client to invite-only channel
[ ] TOPIC - view or change channel topic (restricted to operators if mode +t)
[ ] MODE  - implement all 5 modes:
            i : invite-only on/off
            t : topic restricted to operators on/off
            k : set/remove channel password
            o : give/take operator privilege
            l : set/remove user limit

IRC NUMERIC REPLIES
[ ] 001 RPL_WELCOME        - already done
[ ] 353 RPL_NAMREPLY       - list of users in channel after JOIN
[ ] 366 RPL_ENDOFNAMES     - end of NAMES list
[ ] 331 RPL_NOTOPIC        - no topic set
[ ] 332 RPL_TOPIC          - topic on JOIN
[ ] 431 ERR_NONICKNAMEGIVEN - NICK with no argument
[ ] 432 ERR_ERRONEUSNICKNAME - invalid nick characters
[ ] 433 ERR_NICKNAMEINUSE  - nick already taken
[ ] 441 ERR_USERNOTINCHANNEL - KICK target not in channel
[ ] 442 ERR_NOTONCHANNEL   - client not in channel
[ ] 461 ERR_NEEDMOREPARAMS - missing parameters
[ ] 471 ERR_CHANNELISFULL  - channel reached user limit
[ ] 473 ERR_INVITEONLYCHAN - channel is invite only
[ ] 475 ERR_BADCHANNELKEY  - wrong channel password
[ ] 482 ERR_CHANOPRIVSNEEDED - not an operator

BONUS
-----
[ ] File transfer (DCC SEND)
[ ] IRC Bot that responds to commands in a channel
        example commands: !time, !help, !echo <message>