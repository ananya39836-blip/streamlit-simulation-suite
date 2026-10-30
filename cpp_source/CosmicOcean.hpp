#pragma once

#include "Common.hpp"
#include <vector>
#include <string>

namespace CosmicRobotics {

// Cosmic Marine Biota swimming in the "Sea of Unobservable Universe"
enum CosmicCreatureType {
    CREATURE_VACUUM_MANTA = 0,
    CREATURE_GRAVITON_EEL,
    CREATURE_TACHYON_SIPHONOPHORE
};

struct CosmicFish {
    CosmicCreatureType type;
    Vec2f position;
    Vec2f velocity;
    float headingAngle;
    float size;
    float finOscillation;
    float finSpeed;
    float glowPhase;
    Gdiplus::Color glowColor;
    std::wstring speciesName;
    std::wstring energySource;
};

// Extraterrestrial Presence (Elder Alien holding Earth / planetary worlds)
struct AlienPresence {
    Vec2f position;
    float scale;
    float breathingTimer;
    float eyeGlowIntensity;
    float globeRotation;
    bool cvTracked;
    float cvConfidence;
    std::wstring entityClass;
    std::wstring threatLevel;
};

class CosmicOcean {
public:
    CosmicOcean();
    ~CosmicOcean();

    void initialize(float arenaW, float arenaH);
    void update(float dt);
    void render(Gdiplus::Graphics& g, float x, float y, float w, float h);

    const AlienPresence& getAlien() const { return m_alien; }
    const std::vector<CosmicFish>& getFish() const { return m_fish; }

    void triggerSpacetimeTidePulse();

private:
    void renderOceanBackground(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderCosmicFish(Gdiplus::Graphics& g, const CosmicFish& fish);
    void renderAlienPresence(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderAlienEarth(Gdiplus::Graphics& g, float ex, float ey, float radius);
    void renderCVOverlay(Gdiplus::Graphics& g, float bx, float by, float bw, float bh, const wchar_t* label, float conf);
    void renderOutOfSeaPhenomena(Gdiplus::Graphics& g, float x, float y, float w, float h);

    std::vector<CosmicFish> m_fish;
    AlienPresence m_alien;

    float m_oceanTime;
    float m_tideStrength;
    float m_waveGridOffset;
    float m_arenaW;
    float m_arenaH;

    // Fluid dynamics wave nodes for spacetime ether
    static const int WAVE_NODES_X = 24;
    static const int WAVE_NODES_Y = 16;
    float m_waveHeight[WAVE_NODES_X][WAVE_NODES_Y];
};

} // namespace CosmicRobotics
