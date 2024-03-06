#ifndef STREAM_H
#define STREAM_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <SDL2/SDL.h>


//std::vector<char> audioBuffer;
//std::mutex audioMutex;
//std::condition_variable audioCV;
//bool quit = false;
//bool readyToSend = false;

// Struktura nagłówka WAV
#pragma pack(push, 1)
struct WavHeader {
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2ID[4];
    uint32_t subchunk2Size;
};
#pragma pack(pop)


class Stream
{
private:
    int sock;
    int streamSock;
    int PORT;
    char* ip;
    struct sockaddr_in clientAddr;
public:
    Stream();
    int connectToStream();
    void playAudio();
    void writeWavHeader(std::vector<char>& buffer, uint32_t dataSize, uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample);
    int receiveData();
    void quitStream();

    static bool quit;
};

#endif // STREAM_H
