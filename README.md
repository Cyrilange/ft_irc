_This project has been created as part of the 42 curriculum by csalamit , mariogo2 , aternero._

# <u>Description</u>

This project is an IRC server.


This project implements a basic IRC server using C++98 and network sockets. It allows multiple clients to connect, communicate in channels, and exchange messages in real time following the IRC protocol. The goal is to understand network programming and client-server architectures.

The learning objective in this project is to gain practical experience with low-level networking, event-driven programming, and protocol handling. We focus on building a robust server capable of managing multiple clients and understanding how real-time communication systems operate.





# <u>Instructions</u>

###### This project (ircserv) is to develop an IRC server using the C++ 98 standard, it means neither an IRC client nor an implementation of a server-to-server communication

###### to start you shall use ./ircserv [port] [password] 
###### Then open a new terminal ( you can open multiple) with nc then the port , ex : nc [localhost || ipv4] 6667
- Then, we have to implement the commands that are specific to channel
operators:
  - KICK - Eject a client from the channel
  - INVITE - Invite a client to a channel
  - TOPIC - Change or view the channel topic
  - MODE - Change the channel’s mode:
    - i: Set/remove Invite-only channel
    - t: Set/remove the restrictions of the TOPIC command to channel operators
    - k: Set/remove the channel key (password)
    - o: Give/take channel operator privilege
    - l: Set/remove the user limit to channel


    First you need to put the pass , then make a nick and a user 
    you can send message via privmsg [name to send ] [message ]
    you can also join a channel 


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

