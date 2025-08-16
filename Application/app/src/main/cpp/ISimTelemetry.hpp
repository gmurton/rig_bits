#pragma once

class ISimTelemetry {
public:
    virtual ~ISimTelemetry() = default;

    // Update engine RPM value
    virtual void updateRPM(float rpm) = 0;

    // Update ABS activation status (true if active, false otherwise)
    virtual void updateABS(bool absActive) = 0;

    // Add more telemetry update methods as needed, e.g.:
    // virtual void updateSpeed(float speed) = 0;
    // virtual void updateGear(int gear) = 0;
    // virtual void updateThrottle(float throttle) = 0;
};

// Interface for providing configuration to the telemetry reader
class ITelemetryConfig {
public:
    virtual ~ITelemetryConfig() = default;

    // Example: Get the update interval in milliseconds
    virtual int getUpdateIntervalMs() const = 0;

    // Example: Get the source of telemetry data (e.g., file path, network address)
    virtual const char* getDataSource() const = 0;

    // Add more configuration methods as needed
};