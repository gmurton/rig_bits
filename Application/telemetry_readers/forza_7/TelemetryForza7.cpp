#include "TelemetryForza7.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>      // Add this for timing
#include <thread>      // Already included in header, but needed for sleep
#include <fcntl.h>     // Add this for fcntl

// Constructor
TelemetryForza7::TelemetryForza7()
    : telemetryListener(nullptr),
    running(false),
    lastGear(0),
    lastAbsState(false) 
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
        auto sourceIp = telemetrySource.substr(0, colon);
        auto sourcePort = std::stoi(telemetrySource.substr(colon + 1));

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

        // Make socket non-blocking
        int flags = fcntl(udpSocket, F_GETFL, 0);
        if (flags != -1) {
            fcntl(udpSocket, F_SETFL, flags | O_NONBLOCK);
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
    constexpr size_t forzaDashPacketSize = 324;
    char buffer[bufferSize];

    using clock = std::chrono::steady_clock;
    auto lastHeartbeat = clock::now();

    while (running) 
    {
        ssize_t received = recvfrom(udpSocket, buffer, bufferSize, 0, nullptr, nullptr);
        if (received > 0 && received == forzaDashPacketSize) 
        {
            // Offsets in bytes (Dash format)
            // - CurrentEngineRpm
            constexpr size_t RPM_OFFSET   = 12;
            // - Gear
            constexpr size_t GEAR_OFFSET  = 312;
            // - NormalizedAIBrakeDifference used as an indicator for ABS activation
            // ABS is active if this value is non-zero
            // This is a crude way to detect ABS activation as there is no direct ABS flag in Forza 7 telemetry
            constexpr size_t ABS_OFFSET   = 315;

            float rpm = readFloat(buffer, RPM_OFFSET);
            uint8_t gear = readUint8_t(buffer, GEAR_OFFSET);
            int8_t absVal = readInt8_t(buffer, ABS_OFFSET);

            if(telemetryListener)
            {
                telemetryListener->updateRPM(rpm);
                if(gear != lastGear)
                {
                    telemetryListener->updateGear(static_cast<int32_t>(gear));
                    lastGear = gear;
                }
                bool absActive = (absVal != 0); // crude ABS detection
                if(lastAbsState != absActive)
                {
                    telemetryListener->updateABS(absActive);
                    lastAbsState = absActive;
                }
            }

            std::cout << "RPM: " << rpm
                    << " | Gear: " << (int)gear
                    << " | ABS: " << absVal
                    << "\n";        
        }

        // Heartbeat every second
        auto now = clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastHeartbeat).count() >= 1) {
            std::cout << "[Heartbeat] TelemetryForza7 running..." << std::endl;
            if (telemetryListener)
            {
                // TODO: Change the state based on actual telemetry state
                // For now, we assume we are connected
                telemetryListener->heartbeat(
                    ISimTelemetry::TelemetryState::Connected, 
                    std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
                ); 
            }
            lastHeartbeat = now;
        }

        // Sleep a bit to avoid busy loop
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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



float TelemetryForza7::readFloat(const char* buffer, size_t offset) {
    float value;
    std::memcpy(&value, buffer + offset, sizeof(float));
    return value;
}

uint8_t TelemetryForza7::readUint8_t(const char* buffer, size_t offset) {
    return static_cast<uint8_t>(buffer[offset]);
}

int8_t TelemetryForza7::readInt8_t(const char* buffer, size_t offset) {
    return static_cast<int8_t>(buffer[offset]);
}

