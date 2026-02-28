## This project has been created as part of the 42 curriculum by csalamit , mariogo2 , aternero.

# <u>Description</u>

This project is an IRC server.


This project implements a basic IRC server using C++98 and network sockets. It allows multiple clients to connect, communicate in channels, and exchange messages in real time following the IRC protocol. The goal is to understand network programming and client-server architectures.

The learning objective in this project is to gain practical experience with low-level networking, event-driven programming, and protocol handling. We focus on building a robust server capable of managing multiple clients and understanding how real-time communication systems operate.





# <u>Instructions</u>

###### This project (ircserv) is to develop an IRC server using the C++ 98 standard, it means neither an IRC client nor an implementation of a server-to-server communication

###### Externals functions we have avalaible are :
socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept,
htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa,
inet_ntop, send, recv, signal, sigaction,
sigemptyset, sigfillset, sigaddset, sigdelset,
sigismember, lseek, fstat, fcntl, poll (or
equivalent)

the executable must be: <b>./ircserv "port" "password"</b>

<b>Port</b>: The port number on which we listen for incoming IRC connections.

<b>Password</b>: The connection password. It will be needed by any IRC client that tries to connect to the server.

The server must be capable of handling multiple clients simultaneously without hanging.

Forking is prohibited. All I/O operations must be non-blocking.

Only 1 poll() (or equivalent) can be used for handling all these operations (read,
write, but also listen, and so forth).

Several IRC clients exist. We chose one of them as a reference. Our reference client will be used during the evaluation process.

Our reference client must be able to connect to the server without encountering any error.

Communication between client and server has to be done via TCP/IP (v4 or v6).

Using the reference client with the server must be similar to using it with any official IRC server. However, we only have to implement the following features:

- We must be able to authenticate, set a nickname, a username, join a channel,
send and receive private messages using the reference client.
- All the messages sent from one client to a channel have to be forwarded to
every other client that joined the channel.
- We must have operators and regular users.
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


# <u>Resources</u>

## Video
[C++ Network Programming Part 1: Sockets](https://youtu.be/gntyAFoZp-E?si=raChT57wca1tiBe0)

[C++ Network Programming Part 2: TCP & UDP](https://youtu.be/sXW_sNGvqcU?si=v8CVinLbDxaBfFXF) 

[Client & Server Model explained](https://youtu.be/x6tKd1XBwZw?si=CtO3f5mgt3nbZZ5v)

## Read

[Guide for Networking](https://beej.us/guide/bgnet/)

[Internet relay chat protocole : RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459)

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


For this project we used Github with differents branchs 
Create Branch : git switch -c "name of branch "
Change branch : git switch " name of branche "
push on branch : git push --set-upstream origin "Name of branch"