#ifndef CLIENT_H
#define CLIENT_H

#include "sender.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

using namespace std;

class Client
{
private:
    int clientSock;
    sockaddr_in serverAddr{};
    int PORT = 12345;
    const char* ip = "127.0.0.1";


public:
    Client();
    int connectToServer();
    void disconnectFromServer();
    int addToQueue(string);
    int skipSong();
    string getAvaiableSongs();
    int sendSong(string songPath);
    //int connectToServerQT();
};

#endif // CLIENT_H
