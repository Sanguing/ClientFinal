#include "sender.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

Sender::Sender(const char* ip, string songPath) {
    this->ip = ip;
    this->songPath = songPath;
}

int Sender::connectToServer() {
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Creating socket failed");
        return 1;
    }

    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_addr.s_addr = inet_addr(this->ip);
    this->serverAddr.sin_port = htons(this->PORT);

    int connectResult = connect(this->sock, (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    if (connectResult == -1) {
        perror("Connecting to server failed");
        close(this->sock);
        return 1;
    }

    cout << "Connected to server (add song)" << endl;

    return 0;
}

int Sender::sendSong() {
    const int chunkSize = 1024 * 1024; // 1MB chunk size

    ifstream inputFile(this->songPath, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Unable to open file: " << this->songPath << endl;
        return 1;
    }

    // Get the size of the input file
    inputFile.seekg(0, ios::end);
    streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    // Calculate the number of parts
    int numParts = ((int)fileSize + chunkSize - 1) / chunkSize;

    // Read file in chunks and write each chunk to a separate file
    char* buffer = new char[chunkSize];
    for (int partIndex = 0; partIndex < numParts; ++partIndex) {
        // Determine the chunk size for the current part
        int currentChunkSize = min(chunkSize, (int)(fileSize - inputFile.tellg()));

        inputFile.read(buffer, currentChunkSize);

        if (send(this->sock, buffer, currentChunkSize, 0) == -1) {
            perror("Sending file to server failed");
            close(this->sock);
            delete[] buffer;
            return 1;
        }
    }

    inputFile.close();
    delete[] buffer;

    cout << "File split into " << numParts << " parts." << endl;

    return 0;
}

void Sender::disconnectFromServer() {
    close(this->sock);
    cout << "Connection closed" << endl;
}
