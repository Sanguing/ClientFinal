#include "stream.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <SDL2/SDL.h>
#include <thread>
#include <vector>
#include <condition_variable>

std::vector<char> audioBuffer;
std::mutex audioMutex;
std::condition_variable audioCV;
// bool quit = false;
bool Stream::quit;
bool readyToSend = false;

Stream::Stream() {
    this->PORT = 12343;
    //Stream::quit = false;
}

int Stream::connectToStream() {
    Stream::quit = false;
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Creating socket failed");
        return 1;
    }

    this->clientAddr.sin_family = AF_INET;
    this->clientAddr.sin_addr.s_addr = INADDR_ANY;
    this->clientAddr.sin_port = htons(this->PORT);

    if (bind(this->sock, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == -1) {
        perror("Binding address to socket failed");
        close(this->sock);
        return 1;
    }

    if (listen(this->sock, 1) == -1) {
        perror("Listening error");
        close(this->sock);
        return 1;
    }

    std::cout << "Klient czeka na stream na porcie " << this->PORT << "..." << std::endl;

    // Accepting connection
    struct sockaddr_in serverAddr;
    socklen_t serverAddrSize = sizeof(serverAddr);
    this->streamSock = accept(this->sock, (struct sockaddr*)&serverAddr, &serverAddrSize);
    if (this->streamSock == -1) {
        perror("Error during connection accepting");
        close(this->sock);
        return 1;
    }

    std::cout << "Połączenie zaakceptowane." << std::endl;

    // Receiving music in loop
    this->receiveData();

    return 0;
}

void Stream::playAudio() {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wavSpec;
    wavSpec.freq = 44100;
    wavSpec.format = AUDIO_S16SYS;
    wavSpec.channels = 2;
    wavSpec.samples = 4096;
    wavSpec.callback = nullptr;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (device == 0) {
        std::cerr << "Opening audio devices failed: " << SDL_GetError() << std::endl;
        return;
    }

    while (!quit) {
        std::unique_lock<std::mutex> lock(audioMutex);
        audioCV.wait(lock, [] { return readyToSend || quit; }); // czekaj na flagę gotowości lub zakończenia

        if (!quit) {
            size_t bufferSize = audioBuffer.size();
            SDL_QueueAudio(device, audioBuffer.data(), bufferSize);
            audioBuffer.clear();
            lock.unlock();
            SDL_PauseAudioDevice(device, 0);
            SDL_Delay(100);
            readyToSend = false;
        }
    }

    SDL_CloseAudioDevice(device);
    SDL_Quit();
}

void Stream::writeWavHeader(std::vector<char> &buffer, uint32_t dataSize, uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample) {
    WavHeader header;
    strncpy(header.chunkID, "RIFF", 4);
    strncpy(header.format, "WAVE", 4);
    strncpy(header.subchunk1ID, "fmt", 4);
    header.subchunk1Size = 16;
    header.audioFormat = 1; // PCM
    header.numChannels = numChannels;
    header.sampleRate = sampleRate;
    header.bitsPerSample = bitsPerSample;
    header.byteRate = sampleRate * numChannels * bitsPerSample / 8;
    header.blockAlign = numChannels * bitsPerSample / 8;
    strncpy(header.subchunk2ID, "data", 4);
    header.subchunk2Size = dataSize;

    // Dodaj nagłówek do bufora
    buffer.insert(buffer.begin(), reinterpret_cast<char*>(&header), reinterpret_cast<char*>(&header) + sizeof(header));
}

int Stream::receiveData() {
    std::thread audioThread(&Stream::playAudio, std::ref(*this));

    int partIndex = 0;
    while (!quit) {
        int currentChunkSize;
        int bytesReceived = recv(this->streamSock, &currentChunkSize, sizeof(currentChunkSize), 0);
        if (bytesReceived <= 0) {
            break; // Zakończ jeśli nie można odebrać danych
        }

        // Odbieranie danych i dodawanie ich do bufora
        int totalBytesReceived = 0;
        while (totalBytesReceived < currentChunkSize) {
            char buffer[1024];
            int bytesRead = recv(this->streamSock, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) {
                std::cerr << "Receiving data error" << std::endl;
                close(this->streamSock);
                close(this->sock);
                return 1;
            }
            std::lock_guard<std::mutex> lock(audioMutex);
            audioBuffer.insert(audioBuffer.end(), buffer, buffer + bytesRead);
            totalBytesReceived += bytesRead;
        }

        // Dodanie nagłówka do bufora
        const uint32_t sampleRate = 44100; // Częstotliwość próbkowania
        const uint16_t numChannels = 2; // Liczba kanałów
        const uint16_t bitsPerSample = 16; // Liczba bitów na próbkę
        this->writeWavHeader(audioBuffer, currentChunkSize, sampleRate, numChannels, bitsPerSample);

        std::cout << "Odebrano część: " << partIndex++ << std::endl;
        readyToSend = true; // Ustaw flagę gotowości do wysłania
        audioCV.notify_one();
    }

    // Zamknięcie gniazda
    close(this->streamSock);
    close(this->sock);

    std::cout << "Zakończono połączenie" << std::endl;
    quit = true;
    audioCV.notify_one();
    audioThread.join();

    return 0;
}

void Stream::quitStream() {
    quit = true;
}
