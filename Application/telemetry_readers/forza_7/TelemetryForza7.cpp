#include "TelemetryForza7.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Constructor
TelemetryForza7::TelemetryForza7()
    : telemetryListener(nullptr)
{
    // Initialization code here
}

// Destructor
TelemetryForza7::~TelemetryForza7()
{
    // Cleanup code here
    disconnect();
}

// Configure telemetry connection
bool TelemetryForza7::configureTelemetry(ISimTelemetry* listener, std::string telemetrySource)
{
    bool retVal = false;
    if (listener == nullptr || telemetrySource.empty()) 
    {
        std::cerr << "Invalid listener or telemetry source." << std::endl;
    }
    else
    {
        telemetryListener = listener;

        // Parse telemetrySource as "ip:port"
        size_t colon = telemetrySource.find(':');
        if (colon == std::string::npos) 
        {
            std::cerr << "Telemetry source must be in format ip:port" << std::endl;
            return false;
        }
        sourceIp = telemetrySource.substr(0, colon);
        sourcePort = std::stoi(telemetrySource.substr(colon + 1));

        // Create UDP socket
        udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpSocket < 0) 
        {
            std::cerr << "Failed to create UDP socket" << std::endl;
            return false;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(sourcePort);

        if (bind(udpSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
        {
            std::cerr << "Failed to bind UDP socket" << std::endl;
            close(udpSocket);
            udpSocket = -1;
            return false;
        }

        running = true;
        readerThread = std::thread(&TelemetryForza7::udpReadLoop, this);

        retVal = true;    
    }

    return retVal;
}

void TelemetryForza7::udpReadLoop()
{
    constexpr size_t bufferSize = 2048;
    char buffer[bufferSize];

    while (running) {
        ssize_t received = recvfrom(udpSocket, buffer, bufferSize, 0, nullptr, nullptr);
        if (received > 0) 
        {
            // Process received data here
            // Example: telemetryListener->onTelemetryData(buffer, received);
        }
    }
}

// Disconnect telemetry
void TelemetryForza7::disconnect()
{
    if(running)
    {
        running = false;
        if (readerThread.joinable())
        {
            readerThread.join();
        }
        if (udpSocket != -1) 
        {
            close(udpSocket);
            udpSocket = -1;
        }
    }
    telemetryListener = nullptr;
}

// Add this private method to the class definition in the header:
// void TelemetryForza7::setTelemetryListener(ISimTelemetry* listener)
// {
//     telemetryListener = listener;
// }