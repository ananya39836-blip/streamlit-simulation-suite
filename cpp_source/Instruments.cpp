#include "Instruments.hpp"
#include <cstdio>
#include <cmath>

namespace CosmicRobotics {

Instruments::Instruments()
    : m_viewMode(VIEW_COSMIC_OCEAN)
    , m_distanceGly(46.52) // Positioned right at the 46.5 Gly unobservable boundary
    , m_distanceMpc(14263.0)
    , m_speedFractionOfC(0.994f)
    , m_targetSpeedFractionOfC(0.994f)
    , m_lorentzGamma(9.14f)
    , m_needleAngle(0.0f)
    , m_speedPresetIndex(2)
    , m_cosmicWebPulse(0.0f)
    , m_lidarSweepAngle(0.0f)
    , m_timeElapsed(0.0f)
{
    m_ocean = std::unique_ptr<CosmicOcean>(new CosmicOcean());
    for (int i = 0; i < 8; ++i) {
        m_odometerRollAnim[i] = 0.0f;
    }
}

Instruments::~Instruments() {}

void Instruments::initialize() {
    m_ocean->initialize(800.0f, 400.0f);
    m_orbitBodies.clear();

    // Multiscale orbital hierarchy matching user's cosmological diagrams (Image 2, 3, 4)
    // 1. Inner Solar planetary orbit
    OrbitBody b1;
    b1.name = L"Solar/Terrestrial Orbit";
    b1.orbitRadius = 26.0f;
    b1.orbitalSpeed = 1.4f;
    b1.currentAngle = 0.0f;
    b1.bodyRadius = 3.5f;
    b1.color = Gdiplus::Color(255, 100, 200, 255);
    b1.isObservableBoundary = false;
    b1.scaleLabel = L"1 AU (1.5e8 km)";
    m_orbitBodies.push_back(b1);

    // 2. Milky Way Galactic Rotation
    OrbitBody b2;
    b2.name = L"Milky Way Spiral Arm";
    b2.orbitRadius = 55.0f;
    b2.orbitalSpeed = 0.65f;
    b2.currentAngle = 1.2f;
    b2.bodyRadius = 5.0f;
    b2.color = Gdiplus::Color(255, 255, 220, 100);
    b2.isObservableBoundary = false;
    b2.scaleLabel = L"26,000 ly (8 kpc)";
    m_orbitBodies.push_back(b2);

    // 3. Local Group / Andromeda M31 Orbit
    OrbitBody b3;
    b3.name = L"M31 Local Group Infall";
    b3.orbitRadius = 90.0f;
    b3.orbitalSpeed = 0.32f;
    b3.currentAngle = 2.8f;
    b3.bodyRadius = 6.0f;
    b3.color = Gdiplus::Color(255, 255, 120, 200);
    b3.isObservableBoundary = false;
    b3.scaleLabel = L"2.5 Mly (770 kpc)";
    m_orbitBodies.push_back(b3);

    // 4. Virgo Supercluster / Great Attractor
    OrbitBody b4;
    b4.name = L"Virgo Supercluster Flow";
    b4.orbitRadius = 135.0f;
    b4.orbitalSpeed = 0.15f;
    b4.currentAngle = 4.2f;
    b4.bodyRadius = 7.0f;
    b4.color = Gdiplus::Color(255, 120, 255, 160);
    b4.isObservableBoundary = false;
    b4.scaleLabel = L"54 Mly (16.5 Mpc)";
    m_orbitBodies.push_back(b4);

    // 5. Observable Universe Limit (93 Billion Light Years Sphere - Image 3)
    OrbitBody b5;
    b5.name = L"OBSERVABLE HORIZON LIMIT";
    b5.orbitRadius = 180.0f;
    b5.orbitalSpeed = 0.05f;
    b5.currentAngle = 0.0f;
    b5.bodyRadius = 9.0f;
    b5.color = Gdiplus::Color(255, 0, 255, 255);
    b5.isObservableBoundary = true;
    b5.scaleLabel = L"93.0 Gly (28.5 Gpc)";
    m_orbitBodies.push_back(b5);

    m_lidarPoints.reserve(180);
}

void Instruments::toggleViewMode() {
    m_viewMode = (m_viewMode == VIEW_COSMIC_ORBITS) ? VIEW_COSMIC_OCEAN : VIEW_COSMIC_ORBITS;
}

void Instruments::addDistance(double deltaGly) {
    m_distanceGly += deltaGly;
    m_distanceMpc = m_distanceGly * 1000.0 / PARSEC_TO_LY;
}

void Instruments::setTargetSpeedFractionOfC(float fracC) {
    m_targetSpeedFractionOfC = clampf(fracC, 0.01f, 3.5f);
}

void Instruments::cycleSpeedPreset() {
    m_speedPresetIndex = (m_speedPresetIndex + 1) % 4;
    switch (m_speedPresetIndex) {
        case 0: setTargetSpeedFractionOfC(0.12f); break; // Sublight Cruise
        case 1: setTargetSpeedFractionOfC(0.65f); break; // High Relativistic
        case 2: setTargetSpeedFractionOfC(0.994f); break;// Extreme Relativistic
        case 3: setTargetSpeedFractionOfC(2.45f); break; // Metric Spacetime Warp
    }
}

void Instruments::triggerLidarPulse() {
    for (int i = 0; i < 32; ++i) {
        LidarPoint pt;
        pt.angle = randFloat(0.0f, TWO_PI);
        pt.distance = randFloat(20.0f, 175.0f);
        pt.intensity = randFloat(0.6f, 1.0f);
        pt.life = 1.0f;
        m_lidarPoints.push_back(pt);
    }
}

void Instruments::updateOdometer(float dt) {
    double effectiveSpeed = (double)m_speedFractionOfC;
    double hubbleFactor = (m_distanceGly > 46.5) ? 0.08 : 0.02;
    double deltaGly = (effectiveSpeed * 0.05 + m_distanceGly * hubbleFactor * 0.001) * dt;
    
    addDistance(deltaGly);

    double distVal = m_distanceGly;
    for (int i = 0; i < 8; ++i) {
        double divisor = std::pow(10.0, 4 - i);
        double digitVal = std::fmod(distVal / divisor, 10.0);
        m_odometerRollAnim[i] = lerpf(m_odometerRollAnim[i], static_cast<float>(digitVal), dt * 10.0f);
    }
}

void Instruments::updateSpeedometer(float dt) {
    m_speedFractionOfC = lerpf(m_speedFractionOfC, m_targetSpeedFractionOfC, dt * 2.5f);

    float beta = clampf(m_speedFractionOfC, 0.0f, 0.9995f);
    m_lorentzGamma = 1.0f / std::sqrt(1.0f - beta * beta);
    if (m_speedFractionOfC > 1.0f) {
        m_lorentzGamma = m_speedFractionOfC * 8.5f;
    }

    float normSpeed = clampf(m_speedFractionOfC / 2.5f, 0.0f, 1.0f);
    float targetAngle = -2.1f + normSpeed * 4.2f;
    m_needleAngle = lerpf(m_needleAngle, targetAngle, dt * 6.0f);
}

void Instruments::updateOrbitSimulator(float dt) {
    for (auto& body : m_orbitBodies) {
        body.currentAngle += body.orbitalSpeed * dt * 0.7f;
        if (body.currentAngle > TWO_PI) {
            body.currentAngle -= TWO_PI;
        }
    }
    m_cosmicWebPulse = std::sin(m_timeElapsed * 1.8f) * 0.5f + 0.5f;
}

void Instruments::updateLidar(float dt) {
    m_lidarSweepAngle += 2.8f * dt;
    if (m_lidarSweepAngle > TWO_PI) {
        m_lidarSweepAngle -= TWO_PI;
    }

    for (size_t i = 0; i < m_lidarPoints.size(); ++i) {
        m_lidarPoints[i].life -= dt * 0.8f;
    }
    
    if (rand() % 4 == 0 && m_lidarPoints.size() < 120) {
        LidarPoint pt;
        pt.angle = m_lidarSweepAngle;
        pt.distance = randFloat(30.0f, 160.0f);
        pt.intensity = 1.0f;
        pt.life = 1.0f;
        m_lidarPoints.push_back(pt);
    }
}

void Instruments::update(float dt) {
    m_timeElapsed += dt;
    m_ocean->update(dt);
    updateOdometer(dt);
    updateSpeedometer(dt);
    updateOrbitSimulator(dt);
    updateLidar(dt);
}

void Instruments::render(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // Background panel for Right Half
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 10, 14, 24));
    g.FillRectangle(&bgBrush, x, y, w, h);

    // Top section: Multi-Scale Orbits OR Sea of Unobservable Universe
    float mapH = h * 0.53f;
    if (m_viewMode == VIEW_COSMIC_OCEAN) {
        m_ocean->render(g, x + 8.0f, y + 8.0f, w - 16.0f, mapH);
        
        // View header overlay
        Gdiplus::Font vFont(L"Consolas", 8.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
        Gdiplus::SolidBrush vBrush(Gdiplus::Color(255, 0, 240, 255));
        g.DrawString(L"[VIEW 1: SEA OF UNOBSERVABLE UNIVERSE & ALIEN OBSERVER] (Press TAB to Toggle)", -1, &vFont, Gdiplus::PointF(x + 14.0f, y + 12.0f), &vBrush);
    } else {
        renderCosmicMapAndOrbits(g, x + 8.0f, y + 8.0f, w - 16.0f, mapH);
    }

    // Middle-left section: Cosmic Odometer
    float instY = y + mapH + 16.0f;
    float instW = (w - 24.0f) * 0.49f;
    float instH = h - (mapH + 24.0f) - 40.0f;
    renderOdometerGauge(g, x + 8.0f, instY, instW, instH);

    // Middle-right section: Relativistic Speedometer
    float speedX = x + 8.0f + instW + 8.0f;
    renderSpeedometerGauge(g, speedX, instY, instW, instH);

    // Bottom banner: LiDAR, CV Controllers & Telemetry
    renderTelemetryBar(g, x + 8.0f, y + h - 36.0f, w - 16.0f, 28.0f);
}

void Instruments::renderCosmicMapAndOrbits(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    Gdiplus::Pen framePen(Gdiplus::Color(255, 30, 60, 95), 1.5f);
    g.DrawRectangle(&framePen, x, y, w, h);

    Gdiplus::Font headerFont(L"Consolas", 8.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 0, 220, 255));
    g.DrawString(L"[VIEW 2: 93 Gly COSMIC HORIZON & ORBITS SIMULATION] (Press TAB to Toggle)", -1, &headerFont, Gdiplus::PointF(x + 14.0f, y + 12.0f), &headerBrush);

    float cx = x + w * 0.5f;
    float cy = y + h * 0.54f;
    float maxR = (h * 0.40f);

    Gdiplus::Pen gridPen(Gdiplus::Color(40, 0, 150, 200), 1.0f);
    for (float r = 25.0f; r <= maxR; r += 30.0f) {
        g.DrawEllipse(&gridPen, cx - r, cy - r, r * 2.0f, r * 2.0f);
    }
    g.DrawLine(&gridPen, cx - maxR, cy, cx + maxR, cy);
    g.DrawLine(&gridPen, cx, cy - maxR, cx, cy + maxR);

    float horizonR = maxR * 0.88f;
    Gdiplus::Pen horizonPen(Gdiplus::Color(220, 0, 240, 255), 2.2f);
    g.DrawEllipse(&horizonPen, cx - horizonR, cy - horizonR, horizonR * 2.0f, horizonR * 2.0f);

    Gdiplus::SolidBrush unobsShade(Gdiplus::Color(45, 120, 0, 180));
    g.FillEllipse(&unobsShade, cx - maxR, cy - maxR, maxR * 2.0f, maxR * 2.0f);

    for (const auto& body : m_orbitBodies) {
        float rad = (body.orbitRadius / 180.0f) * horizonR;
        Gdiplus::Pen orbitPen(Gdiplus::Color(70, body.color.GetR(), body.color.GetG(), body.color.GetB()), 1.2f);
        g.DrawEllipse(&orbitPen, cx - rad, cy - rad, rad * 2.0f, rad * 2.0f);

        float bx = cx + std::cos(body.currentAngle) * rad;
        float by = cy + std::sin(body.currentAngle) * rad;

        Gdiplus::SolidBrush bodyBrush(body.color);
        g.FillEllipse(&bodyBrush, bx - body.bodyRadius, by - body.bodyRadius, body.bodyRadius * 2.0f, body.bodyRadius * 2.0f);

        Gdiplus::Pen reticlePen(Gdiplus::Color(180, 0, 255, 200), 1.0f);
        float rbox = body.bodyRadius + 4.0f;
        g.DrawRectangle(&reticlePen, bx - rbox, by - rbox, rbox * 2.0f, rbox * 2.0f);

        Gdiplus::Font tagFont(L"Consolas", 6.8f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
        Gdiplus::SolidBrush tagBrush(Gdiplus::Color(220, 200, 230, 255));
        g.DrawString(body.name.c_str(), -1, &tagFont, Gdiplus::PointF(bx + rbox + 3.0f, by - 6.0f), &tagBrush);
    }

    renderLidarScanner(g, cx, cy, horizonR);
}

void Instruments::renderLidarScanner(Gdiplus::Graphics& g, float cx, float cy, float maxRadius) {
    float lx = cx + std::cos(m_lidarSweepAngle) * maxRadius;
    float ly = cy + std::sin(m_lidarSweepAngle) * maxRadius;
    
    Gdiplus::Pen sweepPen(Gdiplus::Color(220, 50, 255, 120), 1.8f);
    g.DrawLine(&sweepPen, cx, cy, lx, ly);

    for (const auto& pt : m_lidarPoints) {
        if (pt.life <= 0.05f) continue;
        float px = cx + std::cos(pt.angle) * pt.distance;
        float py = cy + std::sin(pt.angle) * pt.distance;
        
        BYTE alpha = static_cast<BYTE>(clampf(pt.life * 255.0f, 0.0f, 255.0f));
        Gdiplus::SolidBrush hitBrush(Gdiplus::Color(alpha, 100, 255, 150));
        g.FillEllipse(&hitBrush, px - 2.5f, py - 2.5f, 5.0f, 5.0f);
    }
}

void Instruments::renderOdometerGauge(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    Gdiplus::SolidBrush panelBg(Gdiplus::Color(255, 16, 22, 34));
    g.FillRectangle(&panelBg, x, y, w, h);

    Gdiplus::Pen borderPen(Gdiplus::Color(255, 0, 160, 220), 1.5f);
    g.DrawRectangle(&borderPen, x, y, w, h);

    Gdiplus::Font titleFont(L"Consolas", 8.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 0, 220, 255));
    g.DrawString(L"COSMIC ODOMETER (ODOS-METRIC)", -1, &titleFont, Gdiplus::PointF(x + 8.0f, y + 5.0f), &titleBrush);

    float drumY = y + 24.0f;
    float drumH = 38.0f;
    float drumW = w - 16.0f;
    Gdiplus::SolidBrush drumBg(Gdiplus::Color(255, 5, 8, 12));
    g.FillRectangle(&drumBg, x + 8.0f, drumY, drumW, drumH);
    
    Gdiplus::Pen drumBorder(Gdiplus::Color(255, 70, 90, 110), 1.5f);
    g.DrawRectangle(&drumBorder, x + 8.0f, drumY, drumW, drumH);

    wchar_t distStr[32];
    swprintf_s(distStr, L"%08.2f", m_distanceGly);

    float digitW = drumW / 10.0f;
    Gdiplus::Font drumFont(L"Consolas", 13.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);

    for (int i = 0; i < 9; ++i) {
        float dx = x + 10.0f + i * digitW;
        Gdiplus::Pen sepPen(Gdiplus::Color(255, 30, 40, 55), 1.0f);
        g.DrawLine(&sepPen, dx + digitW - 1.0f, drumY, dx + digitW - 1.0f, drumY + drumH);

        wchar_t ch = distStr[i];
        if (ch == L'.') {
            Gdiplus::SolidBrush decBrush(Gdiplus::Color(255, 255, 180, 0));
            g.DrawString(L".", -1, &drumFont, Gdiplus::PointF(dx + 2.0f, drumY + 6.0f), &decBrush);
        } else {
            bool isFraction = (i >= 6);
            Gdiplus::SolidBrush numBrush(isFraction ? Gdiplus::Color(255, 255, 90, 80) : Gdiplus::Color(255, 240, 245, 255));
            wchar_t s[2] = { ch, 0 };
            g.DrawString(s, -1, &drumFont, Gdiplus::PointF(dx + 2.0f, drumY + 6.0f), &numBrush);
        }
    }

    Gdiplus::Font infoFont(L"Consolas", 7.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush infoBrush(Gdiplus::Color(255, 180, 205, 230));

    wchar_t line1[64];
    swprintf_s(line1, L"PROPER DISTANCE : %.2f Gly", m_distanceGly);
    g.DrawString(line1, -1, &infoFont, Gdiplus::PointF(x + 10.0f, drumY + drumH + 8.0f), &infoBrush);

    wchar_t line2[64];
    swprintf_s(line2, L"COMOVING DIST   : %.0f Mpc", m_distanceMpc);
    g.DrawString(line2, -1, &infoFont, Gdiplus::PointF(x + 10.0f, drumY + drumH + 22.0f), &infoBrush);

    wchar_t line3[64];
    bool beyondHorizon = (m_distanceGly >= 46.5);
    swprintf_s(line3, L"DOMAIN: %s", beyondHorizon ? L"UNOBSERVABLE OCEAN" : L"OBSERVABLE SPHERE");
    Gdiplus::SolidBrush statusBrush(beyondHorizon ? Gdiplus::Color(255, 255, 80, 220) : Gdiplus::Color(255, 0, 255, 160));
    g.DrawString(line3, -1, &infoFont, Gdiplus::PointF(x + 10.0f, drumY + drumH + 36.0f), &statusBrush);
}

void Instruments::renderSpeedometerGauge(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    Gdiplus::SolidBrush panelBg(Gdiplus::Color(255, 16, 22, 34));
    g.FillRectangle(&panelBg, x, y, w, h);

    Gdiplus::Pen borderPen(Gdiplus::Color(255, 255, 140, 0), 1.5f);
    g.DrawRectangle(&borderPen, x, y, w, h);

    Gdiplus::Font titleFont(L"Consolas", 8.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 170, 0));
    g.DrawString(L"TACHOMETER (VELOX-TACHYON)", -1, &titleFont, Gdiplus::PointF(x + 8.0f, y + 5.0f), &titleBrush);

    float gcx = x + w * 0.5f;
    float gcy = y + h * 0.50f;
    float gr = (h * 0.32f);

    Gdiplus::Pen bgArcPen(Gdiplus::Color(255, 35, 45, 60), 5.0f);
    g.DrawArc(&bgArcPen, gcx - gr, gcy - gr, gr * 2.0f, gr * 2.0f, 140.0f, 260.0f);

    float speedFrac = clampf(m_speedFractionOfC / 2.5f, 0.0f, 1.0f);
    float sweepDeg = speedFrac * 260.0f;
    Gdiplus::Color arcCol = (m_speedFractionOfC > 1.0f) ? Gdiplus::Color(255, 255, 60, 200) : Gdiplus::Color(255, 255, 160, 20);
    Gdiplus::Pen activeArcPen(arcCol, 5.0f);
    g.DrawArc(&activeArcPen, gcx - gr, gcy - gr, gr * 2.0f, gr * 2.0f, 140.0f, sweepDeg);

    float nx = gcx + std::cos(m_needleAngle) * (gr - 5.0f);
    float ny = gcy + std::sin(m_needleAngle) * (gr - 5.0f);
    Gdiplus::Pen needlePen(Gdiplus::Color(255, 255, 255, 255), 2.0f);
    g.DrawLine(&needlePen, gcx, gcy, nx, ny);

    Gdiplus::SolidBrush needleCap(Gdiplus::Color(255, 255, 100, 30));
    g.FillEllipse(&needleCap, gcx - 4.0f, gcy - 4.0f, 8.0f, 8.0f);

    Gdiplus::Font speedFont(L"Consolas", 11.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    wchar_t spdBuf[32];
    swprintf_s(spdBuf, L"%.3f c", m_speedFractionOfC);
    Gdiplus::SolidBrush spdBrush(arcCol);
    g.DrawString(spdBuf, -1, &speedFont, Gdiplus::PointF(gcx - 22.0f, gcy + 15.0f), &spdBrush);

    Gdiplus::Font subFont(L"Consolas", 6.8f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 190, 210, 230));

    wchar_t kmBuf[48];
    swprintf_s(kmBuf, L"VELOCITY: %.0f km/s", getSpeedKmPerSec());
    g.DrawString(kmBuf, -1, &subFont, Gdiplus::PointF(x + 10.0f, y + h - 28.0f), &subBrush);

    wchar_t gamBuf[48];
    swprintf_s(gamBuf, L"GAMMA \u03b3: %.2fx", m_lorentzGamma);
    g.DrawString(gamBuf, -1, &subFont, Gdiplus::PointF(x + 10.0f, y + h - 16.0f), &subBrush);
}

void Instruments::renderTelemetryBar(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    Gdiplus::SolidBrush barBg(Gdiplus::Color(255, 12, 16, 26));
    g.FillRectangle(&barBg, x, y, w, h);

    Gdiplus::Pen barPen(Gdiplus::Color(255, 30, 60, 90), 1.0f);
    g.DrawRectangle(&barPen, x, y, w, h);

    Gdiplus::Font barFont(L"Consolas", 7.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 0, 230, 255));

    wchar_t telemetryMsg[128];
    swprintf_s(telemetryMsg, L"[LIDAR 360 ACTIVE] SENSORS: LOCK | CV TRACK: 1 ALIEN + 3 FISH | OCEAN DENSITY: NORMALIZED | CONTROLLER: ARMED");
    g.DrawString(telemetryMsg, -1, &barFont, Gdiplus::PointF(x + 10.0f, y + 7.0f), &textBrush);
}

} // namespace CosmicRobotics
