#ifndef SENDER_H
#define SENDER_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>

using namespace std;

class Sender
{
private:
    const int PORT = 12344;
    const char* ip;
    int sock;
    sockaddr_in serverAddr{};
    string songPath;
public:
    Sender(const char* ip, string songPath);
    int connectToServer();
    int sendSong();
    void disconnectFromServer();
};

#endif // SENDER_H
