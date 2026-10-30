#pragma once

#include "Common.hpp"
#include "CosmicOcean.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CosmicRobotics {

// Orbital entity for the cosmic orbit simulator
struct OrbitBody {
    std::wstring name;
    float orbitRadius;      // in simulation pixels
    float orbitalSpeed;     // radians per second
    float currentAngle;
    float bodyRadius;
    Gdiplus::Color color;
    bool isObservableBoundary;
    std::wstring scaleLabel;
};

// LiDAR scan point
struct LidarPoint {
    float angle;
    float distance;
    float intensity;
    float life; // Fading
};

enum SimulationViewMode {
    VIEW_COSMIC_ORBITS = 0,    // Multi-scale 93 Gly orbit maps (Images 2, 3, 4)
    VIEW_COSMIC_OCEAN = 1      // Sea of unobservable universe, fish, alien presence (Images 1, 5)
};

class Instruments {
public:
    Instruments();
    ~Instruments();

    void initialize();
    void update(float dt);
    
    // Renders the whole scientific instrument suite on the right half
    void render(Gdiplus::Graphics& g, float x, float y, float w, float h);

    // Odometer interface
    double getDistanceGly() const { return m_distanceGly; }
    double getDistanceMpc() const { return m_distanceMpc; }
    void addDistance(double deltaGly);

    // Speedometer interface
    float getSpeedFractionOfC() const { return m_speedFractionOfC; }
    double getSpeedKmPerSec() const { return m_speedFractionOfC * SPEED_OF_LIGHT; }
    float getLorentzGamma() const { return m_lorentzGamma; }
    void setTargetSpeedFractionOfC(float fracC);
    void cycleSpeedPreset();

    // LiDAR & Sensor Controller interface
    void triggerLidarPulse();

    // View mode switching
    void toggleViewMode();
    SimulationViewMode getViewMode() const { return m_viewMode; }

    CosmicOcean& getCosmicOcean() { return *m_ocean; }

private:
    void updateOdometer(float dt);
    void updateSpeedometer(float dt);
    void updateOrbitSimulator(float dt);
    void updateLidar(float dt);

    // Rendering sections
    void renderCosmicMapAndOrbits(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderOdometerGauge(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderSpeedometerGauge(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderLidarScanner(Gdiplus::Graphics& g, float cx, float cy, float maxRadius);
    void renderTelemetryBar(Gdiplus::Graphics& g, float x, float y, float w, float h);

    SimulationViewMode m_viewMode;
    std::unique_ptr<CosmicOcean> m_ocean;

    // Odometer state
    double m_distanceGly;          // In Giga-light-years (e.g., 46.50 Gly)
    double m_distanceMpc;          // In Mega-parsecs
    float m_odometerRollAnim[8];   // Rolling animation interpolation for digits

    // Speedometer state
    float m_speedFractionOfC;      // Current speed (0.0 to 1.0 or beyond for warp)
    float m_targetSpeedFractionOfC;// Desired speed
    float m_lorentzGamma;          // Relativistic dilation factor
    float m_needleAngle;           // In radians
    int m_speedPresetIndex;

    // Orbit simulator
    std::vector<OrbitBody> m_orbitBodies;
    float m_cosmicWebPulse;

    // LiDAR & Sensors
    float m_lidarSweepAngle;
    std::vector<LidarPoint> m_lidarPoints;
    float m_timeElapsed;
};

} // namespace CosmicRobotics
