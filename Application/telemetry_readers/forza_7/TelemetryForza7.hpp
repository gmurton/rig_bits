#pragma once

#include "ISimTelemetry.hpp"

class TelemetryForza7 : public ITelemetryConfig:
    TelemetryForza7();
    virtual ~TelemetryForza7();

    // Override virtual methods from ISimTelemetry
    virtual bool configureTelemetry(ISimTelemetry* listener, std::string telemetrySource) override;
    virtual void disconnect() override;

    // Forza 7 specific methods or members here

private:
    // Private members for internal state
    ISimTelemetry* telemetryListener; // Pointer to the telemetry listener
};
