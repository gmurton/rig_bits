#pragma once

#include "ISimTelemetry.hpp"
#include <atomic>
#include <thread>

class TelemetryForza7 : public ITelemetryConfig {
public:
    TelemetryForza7();
    virtual ~TelemetryForza7();

    // Override virtual methods from ISimTelemetry
    virtual bool configureTelemetry(ISimTelemetry* listener, std::string telemetrySource) override;
    virtual void disconnect() override;

    // Forza 7 specific methods or members here

private:
    // Private members for internal state
    ISimTelemetry* telemetryListener; // Pointer to the telemetry listener
    std::thread readerThread;
    std::atomic<bool> running;
    int udpSocket = -1;
    void udpReadLoop();
    int8_t readInt8_t(const char* buffer, size_t offset);
    uint8_t readUint8_t(const char* buffer, size_t offset);
    float readFloat(const char* buffer, size_t offset);

    uint8_t lastGear; // Last known gear - we only need to notify if changed
    bool lastAbsState; // Last known ABS status

};
