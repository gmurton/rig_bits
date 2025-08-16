#pragma once
#include <cstdint> // For fixed size types e.g. std::uint64_t
#include <string>  // For std::string

// Interface for sim telemetry reader
// This interface defines the methods that a telemetry reader must implement
// to provide telemetry data from a simulation environment.

// This interface is designed to be used with a telemetry reader that connects
// to a simulation environment and retrieves real-time telemetry data such as
// engine RPM, ABS status, and gear selection.

class ISimTelemetry {
public:

    enum class TelemetryState 
    {
        Disconnected, // Not connected to telemetry source
        Connecting,   // Attempting to connect to telemetry source
        Connected,    // Successfully connected to telemetry source
        NoData,       // Connected but no data available
        Error         // An error occurred while connecting or retrieving data
    };
    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~ISimTelemetry() = default;

    // A heartbeat message which contains some simple status info
    // This should be called periodically to indicate the current state of the telemetry reader
    virtual void heartbeat(TelemetryState currentState, std::uint64_t timestamp) = 0;

    // Update engine RPM value
    virtual void updateRPM(double rpm) = 0;

    // Update ABS activation status (true if active, false otherwise)
    virtual void updateABS(bool absActive) = 0;

    // Update the currently selected gear 
    // (int as negative can be reverse gears, 0 for neutral and positive numbers reflecting forward gears)
    virtual void updateGear(std::int32_t gear) = 0;

};

// Interface for providing configuration to the telemetry reader
class ITelemetryConfig {
public:
    virtual ~ITelemetryConfig() = default;

    // Configure the telemetry reader with a data source 
    // e.g. a network address, uri or other identifier
    // Returns true if configuration was successful, false otherwise
    virtual bool configureTelemetry(ISimTelemetry* listener, std::string telemetrySource) = 0;

    // Disconnect from the telemetry source   
    virtual void disconnect() = 0;

    // Add more configuration methods as needed
};