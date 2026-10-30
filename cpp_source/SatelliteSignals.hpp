#pragma once

#include "Common.hpp"
#include <vector>

namespace CosmicRobotics {

struct SignalWave {
    Vec2f origin;
    float currentRadius;
    float maxRadius;
    float expansionSpeed;
    float alpha;
    Gdiplus::Color color;
    bool active;
};

struct TelemetryPacket {
    Vec2f position;
    Vec2f startPos;
    Vec2f targetPos;
    float progress;       // 0.0 to 1.0
    float speed;
    float pulseSize;
    Gdiplus::Color color;
    std::wstring payloadLabel;
    bool active;
};

class SatelliteSignals {
public:
    SatelliteSignals();
    ~SatelliteSignals();

    void initialize(const Vec2f& unobservableOrigin, const Vec2f& observatoryTarget);
    void update(float dt);
    void render(Gdiplus::Graphics& g, float clipX, float clipY, float clipW, float clipH);

    void triggerBurst();
    void setSignalStrength(float dbm);

    // Metrics for display
    float getCarrierFreqGHz() const { return m_carrierFreqGHz; }
    float getSignalToNoiseDB() const { return m_snrDB; }
    float getDecoherenceRate() const { return m_decoherenceRate; }
    double getRedshiftZ() const { return m_redshiftZ; }

private:
    Vec2f m_origin;      // Chronos-Void in unobservable universe
    Vec2f m_target;      // Observatory / Laptop receiver dish
    
    std::vector<SignalWave> m_waves;
    std::vector<TelemetryPacket> m_packets;

    float m_waveSpawnTimer;
    float m_packetSpawnTimer;
    float m_timeElapsed;

    // Signal telemetry
    float m_carrierFreqGHz;
    float m_snrDB;
    float m_decoherenceRate;
    double m_redshiftZ;
    float m_beamIntensity;
};

} // namespace CosmicRobotics
