#include "SatelliteSignals.hpp"

namespace CosmicRobotics {

SatelliteSignals::SatelliteSignals()
    : m_origin(0.0f, 0.0f)
    , m_target(0.0f, 0.0f)
    , m_waveSpawnTimer(0.0f)
    , m_packetSpawnTimer(0.0f)
    , m_timeElapsed(0.0f)
    , m_carrierFreqGHz(248.84f)
    , m_snrDB(38.4f)
    , m_decoherenceRate(0.0014f)
    , m_redshiftZ(1100.8)
    , m_beamIntensity(1.0f)
{
    m_waves.reserve(64);
    m_packets.reserve(64);
}

SatelliteSignals::~SatelliteSignals() {}

void SatelliteSignals::initialize(const Vec2f& unobservableOrigin, const Vec2f& observatoryTarget) {
    m_origin = unobservableOrigin;
    m_target = observatoryTarget;
}

void SatelliteSignals::triggerBurst() {
    m_beamIntensity = 2.5f;

    // Emit intense burst of wavefronts
    for (int i = 0; i < 5; ++i) {
        SignalWave w;
        w.origin = m_origin;
        w.currentRadius = static_cast<float>(i * 18);
        w.maxRadius = 380.0f;
        w.expansionSpeed = 120.0f + i * 20.0f;
        w.alpha = 255.0f;
        w.color = Gdiplus::Color(255, 180, 100, 255); // Radiant purple/cyan
        w.active = true;
        m_waves.push_back(w);
    }
}

void SatelliteSignals::setSignalStrength(float dbm) {
    m_snrDB = dbm;
}

void SatelliteSignals::update(float dt) {
    m_timeElapsed += dt;

    if (m_beamIntensity > 1.0f) {
        m_beamIntensity = lerpf(m_beamIntensity, 1.0f, dt * 2.0f);
    }

    // Spawn regular concentric radio / microwave wavefronts
    m_waveSpawnTimer += dt;
    if (m_waveSpawnTimer > 0.45f) {
        m_waveSpawnTimer = 0.0f;
        SignalWave w;
        w.origin = m_origin;
        w.currentRadius = 10.0f;
        w.maxRadius = 320.0f;
        w.expansionSpeed = 85.0f;
        w.alpha = 200.0f;
        w.color = Gdiplus::Color(255, 120, 180, 255);
        w.active = true;
        m_waves.push_back(w);
    }

    // Update wavefronts
    for (size_t i = 0; i < m_waves.size(); ++i) {
        if (!m_waves[i].active) continue;
        m_waves[i].currentRadius += m_waves[i].expansionSpeed * dt;
        float progress = m_waves[i].currentRadius / m_waves[i].maxRadius;
        m_waves[i].alpha = clampf((1.0f - progress) * 220.0f, 0.0f, 255.0f);
        if (progress >= 1.0f) {
            m_waves[i].active = false;
        }
    }

    // Spawn telemetry data packets streaming from deep space
    m_packetSpawnTimer += dt;
    if (m_packetSpawnTimer > 0.35f) {
        m_packetSpawnTimer = 0.0f;
        TelemetryPacket p;
        p.startPos = m_origin;
        p.targetPos = m_target;
        p.position = m_origin;
        p.progress = 0.0f;
        p.speed = randFloat(0.35f, 0.65f);
        p.pulseSize = randFloat(4.0f, 7.5f);
        p.color = Gdiplus::Color(255, 0, 240, 255); // Cyan quantum packet
        
        static const wchar_t* LABELS[] = {
            L"Q-FLUX", L"ODOM-SYNC", L"HORIZON-TEL", L"DARK-FLOW", L"CMB-DIPOLE"
        };
        p.payloadLabel = LABELS[rand() % 5];
        p.active = true;
        m_packets.push_back(p);
    }

    // Update packets
    for (size_t i = 0; i < m_packets.size(); ++i) {
        if (!m_packets[i].active) continue;
        m_packets[i].progress += m_packets[i].speed * dt;
        
        // Curved flight path representing gravitational deflection across the cosmic web
        float prg = m_packets[i].progress;
        Vec2f linearPos = m_packets[i].startPos + (m_packets[i].targetPos - m_packets[i].startPos) * prg;
        float arcOffset = std::sin(prg * PI) * 35.0f;
        
        m_packets[i].position.x = linearPos.x;
        m_packets[i].position.y = linearPos.y - arcOffset;

        if (prg >= 1.0f) {
            m_packets[i].active = false;
        }
    }

    // Subtle natural fluctuations in telemetry
    m_carrierFreqGHz = 248.84f + std::sin(m_timeElapsed * 1.8f) * 0.12f;
    m_snrDB = 38.4f + std::cos(m_timeElapsed * 0.9f) * 1.5f;
    m_decoherenceRate = 0.0014f + std::sin(m_timeElapsed * 2.5f) * 0.0003f;
}

void SatelliteSignals::render(Gdiplus::Graphics& g, float clipX, float clipY, float clipW, float clipH) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // 1. Draw directed quantum carrier beam curving across spacetime
    Gdiplus::GraphicsPath beamPath;
    float ctrlX = (m_origin.x + m_target.x) * 0.5f;
    float ctrlY = (m_origin.y + m_target.y) * 0.5f - 40.0f;
    beamPath.AddBezier(m_origin.x, m_origin.y, ctrlX, ctrlY - 20.0f, ctrlX, ctrlY + 20.0f, m_target.x, m_target.y);

    // Glowing beam aura
    BYTE auraAlpha = static_cast<BYTE>(clampf(50.0f * m_beamIntensity, 10.0f, 255.0f));
    Gdiplus::Pen beamAuraPen(Gdiplus::Color(auraAlpha, 140, 60, 255), 6.0f);
    g.DrawPath(&beamAuraPen, &beamPath);

    // Core beam filament
    BYTE coreAlpha = static_cast<BYTE>(clampf(180.0f * m_beamIntensity, 40.0f, 255.0f));
    Gdiplus::Pen beamCorePen(Gdiplus::Color(coreAlpha, 0, 230, 255), 2.0f);
    g.DrawPath(&beamCorePen, &beamPath);

    // 2. Draw expanding concentric radio wavefronts from unobservable universe
    for (const auto& w : m_waves) {
        if (!w.active || w.alpha <= 1.0f) continue;
        BYTE a = static_cast<BYTE>(w.alpha);
        Gdiplus::Pen wavePen(Gdiplus::Color(a, w.color.GetR(), w.color.GetG(), w.color.GetB()), 1.8f);
        g.DrawEllipse(&wavePen, w.origin.x - w.currentRadius, w.origin.y - w.currentRadius, w.currentRadius * 2.0f, w.currentRadius * 2.0f);
    }

    // 3. Draw streaming telemetry packets
    for (const auto& p : m_packets) {
        if (!p.active) continue;
        
        // Packet glow halo
        Gdiplus::SolidBrush haloBrush(Gdiplus::Color(70, p.color.GetR(), p.color.GetG(), p.color.GetB()));
        g.FillEllipse(&haloBrush, p.position.x - p.pulseSize * 1.5f, p.position.y - p.pulseSize * 1.5f, p.pulseSize * 3.0f, p.pulseSize * 3.0f);

        // Packet core
        Gdiplus::SolidBrush coreBrush(Gdiplus::Color(255, 255, 255, 255));
        g.FillEllipse(&coreBrush, p.position.x - p.pulseSize * 0.5f, p.position.y - p.pulseSize * 0.5f, p.pulseSize, p.pulseSize);

        // Optional micro tag
        if (p.progress > 0.2f && p.progress < 0.8f) {
            Gdiplus::Font font(L"Consolas", 7.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(190, 0, 240, 255));
            g.DrawString(p.payloadLabel.c_str(), -1, &font, Gdiplus::PointF(p.position.x + 8.0f, p.position.y - 7.0f), &textBrush);
        }
    }

    // 4. Source Satellite Transponder Dish at Unobservable Origin
    Gdiplus::SolidBrush dishBrush(Gdiplus::Color(255, 40, 30, 60));
    g.FillEllipse(&dishBrush, m_origin.x - 14.0f, m_origin.y - 14.0f, 28.0f, 28.0f);
    
    Gdiplus::Pen dishPen(Gdiplus::Color(255, 180, 80, 255), 2.0f);
    g.DrawEllipse(&dishPen, m_origin.x - 14.0f, m_origin.y - 14.0f, 28.0f, 28.0f);

    Gdiplus::SolidBrush dishCore(Gdiplus::Color(255, 255, 200, 50));
    g.FillEllipse(&dishCore, m_origin.x - 4.0f, m_origin.y - 4.0f, 8.0f, 8.0f);

    // 5. Receiving Station Dish at Target
    Gdiplus::SolidBrush recvBrush(Gdiplus::Color(255, 20, 35, 50));
    g.FillEllipse(&recvBrush, m_target.x - 12.0f, m_target.y - 12.0f, 24.0f, 24.0f);

    Gdiplus::Pen recvPen(Gdiplus::Color(255, 0, 220, 255), 2.0f);
    g.DrawEllipse(&recvPen, m_target.x - 12.0f, m_target.y - 12.0f, 24.0f, 24.0f);

    Gdiplus::SolidBrush recvCore(Gdiplus::Color(255, 0, 255, 180));
    g.FillEllipse(&recvCore, m_target.x - 3.5f, m_target.y - 3.5f, 7.0f, 7.0f);
}

} // namespace CosmicRobotics
