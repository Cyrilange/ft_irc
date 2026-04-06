_This project has been created as part of the 42 curriculum by csalamit , mariogo2 , aternero._

# <u>Description</u>

This project is an IRC server.


This project implements a basic IRC server using C++98 and network sockets. It allows multiple clients to connect, communicate in channels, and exchange messages in real time following the IRC protocol. The goal is to understand network programming and client-server architectures.

The learning objective in this project is to gain practical experience with low-level networking, event-driven programming, and protocol handling. We focus on building a robust server capable of managing multiple clients and understanding how real-time communication systems operate.





# <u>Instructions</u>

###### This project (ircserv) is to develop an IRC server using the C++ 98 standard, it means neither an IRC client nor an implementation of a server-to-server communication

###### to start you shall use ./ircserv [port] [password] 
###### Then open a new terminal ( you can open multiple) with nc then the port , ex : nc [localhost || ipv4] 6667
###### you can also use from hexcat 

first you shall enter the password then the nick and finally the user 

the user is : Parameters: <user> <mode> <unused> <realname>  exemple : USER guest 0 * :Ronnie Reagan 

you receive then :

001    RPL_WELCOME
              "Welcome to the Internet Relay Network
               nick!user@host"
002    RPL_YOURHOST
              "Your host is servername, running version ver"
003    RPL_CREATED
              "This server was created date"
004    RPL_MYINFO
              "servername> version> available user modes
               available channel modes"

         - The server sends Replies 001 to 004 to a user upon
           successful registration.

as we did the bonus you will receive as well the bot message 


commands : not sensible like JoiN == join == JOIN == JOin
  -JOIN
   The JOIN command is used by a user to request to start listening to
   the specific channel.  Servers MUST be able to parse arguments in the
   form of a list of target, but SHOULD NOT use lists when sending JOIN
   messages to clients.

   Once a user has joined a channel, he receives information about
   all commands his server receives affecting the channel.  This
   includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
   This allows channel members to keep track of the other channel
   members, as well as channel modes.

  -TOPIC
   The TOPIC command is used to change or view the topic of a channel.
   The topic for channel  is returned if there is no topic
   given.  If the topic parameter is present, the topic for that
   channel will be changed, if this action is allowed for the user
   requesting it.  If the topic parameter is an empty string, the
   topic for that channel will be removed.


  -PRIVMSG with name or channel and finally  message to message 
  -PART with channel and explanation to leave a channel 
  -QUIT to quit 
  - KICK - Eject a client from the channel

  - INVITE
   Parameters: nickname channel

   The INVITE command is used to invite a user to a channel.  The
   parameter nickname is the nickname of the person to be invited to
   the target channel channel.  There is no requirement that the
   channel the target user is being invited to must exist or be a valid
   channel.  However, if the channel exists, only members of the channel
   are allowed to invite other users.  When the channel has invite-only
   flag set, only channel operators may issue INVITE command.

  -TOPIC
   Parameters: channel [ topic ]

   The TOPIC command is used to change or view the topic of a channel.
   The topic for channel  is returned if there is no <topic>
   given.  If the topic parameter is present, the topic for that
   channel will be changed, if this action is allowed for the user
   requesting it.  If the topic parameter is an empty string, the
   topic for that channel will be removed.

  - MODE - Change the channel’s mode: + to add and - to leave 
    - i: Set/remove Invite-only channel
    - t: Set/remove the restrictions of the TOPIC command to channel operators
    - k: Set/remove the channel key (password)
    - o: Give/take channel operator privilege
    - l: Set/remove the user limit to channel


###### crtl + c / crtl + z

c is leaving the app
z the app run on back , to get back on it you have to do fg 


###### BOT
Available commands: !time !help !joke !quote !channel !hi !list";
the bot has multiple jokes, quotes and even hi ( it is random ) 

list on bot.cpp in handleMessage , we used a vector and the push_back() to add sentences 
then we made a function make it random 
  


# <u>Resources</u>

## Video
[C++ Network Programming Part 1: Sockets](https://youtu.be/gntyAFoZp-E?si=raChT57wca1tiBe0)

[C++ Network Programming Part 2: TCP & UDP](https://youtu.be/sXW_sNGvqcU?si=v8CVinLbDxaBfFXF) 

[Client & Server Model explained](https://youtu.be/x6tKd1XBwZw?si=CtO3f5mgt3nbZZ5v)

[Internet Relay Chat.](https://youtu.be/6UbKenFipjo?si=TiY_QJ-r-HUfXJzU)

## Read

[Guide for Networking](https://beej.us/guide/bgnet/)

[Internet relay chat protocole : RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459)

[IRC 2812](https://www.rfc-editor.org/rfc/rfc2812)

[IRC doc](https://modern.ircdocs.horse/)

# <u>Learning Objectives</u>

To complete this project, we needed the following skills and concepts :

- Network programming and socket management
- TCP/IP communication principles
- Event-driven architecture and non-blocking I/O
- Client-server communication models
- Protocol parsing and message handling
- Multiplexing with poll or equivalent mechanisms
- Memory and resource management in C++
- Error handling and robust software design
- Understanding of the IRC protocol structure
- Object-oriented design in C++98

