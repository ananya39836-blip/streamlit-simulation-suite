#include "CosmicOcean.hpp"
#include <cmath>

namespace CosmicRobotics {

CosmicOcean::CosmicOcean()
    : m_oceanTime(0.0f)
    , m_tideStrength(1.0f)
    , m_waveGridOffset(0.0f)
    , m_arenaW(800.0f)
    , m_arenaH(500.0f)
{
    for (int y = 0; y < WAVE_NODES_Y; ++y) {
        for (int x = 0; x < WAVE_NODES_X; ++x) {
            m_waveHeight[x][y] = 0.0f;
        }
    }
}

CosmicOcean::~CosmicOcean() {}

void CosmicOcean::initialize(float arenaW, float arenaH) {
    m_arenaW = arenaW;
    m_arenaH = arenaH;
    m_fish.clear();

    // 1. Vacuum Manta (swims through the cosmic dark fluid)
    CosmicFish m1;
    m1.type = CREATURE_VACUUM_MANTA;
    m1.position = Vec2f(arenaW * 0.28f, arenaH * 0.40f);
    m1.velocity = Vec2f(22.0f, -8.0f);
    m1.headingAngle = -0.35f;
    m1.size = 32.0f;
    m1.finOscillation = 0.0f;
    m1.finSpeed = 3.2f;
    m1.glowPhase = 0.0f;
    m1.glowColor = Gdiplus::Color(255, 0, 240, 255); // Cyan bioluminescence
    m1.speciesName = L"Bioluminescent Vacuum Manta (Manta Cosmica)";
    m1.energySource = L"Zero-Point Energy & Casimir Gradients";
    m_fish.push_back(m1);

    // 2. Graviton Eel (sinuous serpentine swimmer across gravitational waves)
    CosmicFish e1;
    e1.type = CREATURE_GRAVITON_EEL;
    e1.position = Vec2f(arenaW * 0.72f, arenaH * 0.65f);
    e1.velocity = Vec2f(-26.0f, 10.0f);
    e1.headingAngle = 2.75f;
    e1.size = 42.0f;
    e1.finOscillation = 1.4f;
    e1.finSpeed = 4.5f;
    e1.glowPhase = 1.0f;
    e1.glowColor = Gdiplus::Color(255, 180, 80, 255); // Deep violet plasma
    e1.speciesName = L"Graviton Wave Eel (Anguilla Gravitica)";
    e1.energySource = L"Curvature Tensors & Metric Ripple Flux";
    m_fish.push_back(e1);

    // 3. Tachyon Siphonophore (colonial deep-void organism)
    CosmicFish s1;
    s1.type = CREATURE_TACHYON_SIPHONOPHORE;
    s1.position = Vec2f(arenaW * 0.50f, arenaH * 0.80f);
    s1.velocity = Vec2f(12.0f, 16.0f);
    s1.headingAngle = 0.9f;
    s1.size = 28.0f;
    s1.finOscillation = 2.8f;
    s1.finSpeed = 2.4f;
    s1.glowPhase = 2.2f;
    s1.glowColor = Gdiplus::Color(255, 255, 170, 30); // Luminous Amber/Gold
    s1.speciesName = L"Tachyon Siphonophore Colony (Physalia Tachyona)";
    s1.energySource = L"Superluminal Cherenkov Photons";
    m_fish.push_back(s1);

    // Initialize Alien Presence (Elder Cosmological Observer holding Earth in palms)
    m_alien.position = Vec2f(arenaW * 0.52f, arenaH * 0.42f);
    m_alien.scale = 1.0f;
    m_alien.breathingTimer = 0.0f;
    m_alien.eyeGlowIntensity = 1.0f;
    m_alien.globeRotation = 0.0f;
    m_alien.cvTracked = true;
    m_alien.cvConfidence = 0.985f;
    m_alien.entityClass = L"CLASS-IV ELDER COSMIC GUARDIAN";
    m_alien.threatLevel = L"BENEVOLENT OBSERVATIONAL STEWARD";
}

void CosmicOcean::triggerSpacetimeTidePulse() {
    m_tideStrength = 2.8f;
}

void CosmicOcean::update(float dt) {
    m_oceanTime += dt;
    m_waveGridOffset += dt * 0.4f;

    if (m_tideStrength > 1.0f) {
        m_tideStrength = lerpf(m_tideStrength, 1.0f, dt * 1.5f);
    }

    // Update spacetime wave surface nodes
    for (int y = 0; y < WAVE_NODES_Y; ++y) {
        for (int x = 0; x < WAVE_NODES_X; ++x) {
            float u = static_cast<float>(x) * 0.35f + m_waveGridOffset;
            float v = static_cast<float>(y) * 0.35f + m_oceanTime * 0.5f;
            m_waveHeight[x][y] = std::sin(u) * std::cos(v) * 12.0f * m_tideStrength;
        }
    }

    // Update Cosmic Fish swimming dynamics
    for (auto& fish : m_fish) {
        fish.finOscillation += fish.finSpeed * dt;
        fish.glowPhase += dt * 2.2f;

        // Gentle sinusoidal swimming trajectory
        fish.headingAngle += std::sin(fish.finOscillation * 0.6f) * 0.04f;
        fish.velocity.x = std::cos(fish.headingAngle) * 28.0f;
        fish.velocity.y = std::sin(fish.headingAngle) * 28.0f;

        fish.position = fish.position + fish.velocity * dt;

        // Wrap around arena boundaries smoothly
        float pad = 40.0f;
        if (fish.position.x < -pad) fish.position.x = m_arenaW + pad;
        if (fish.position.x > m_arenaW + pad) fish.position.x = -pad;
        if (fish.position.y < -pad) fish.position.y = m_arenaH + pad;
        if (fish.position.y > m_arenaH + pad) fish.position.y = -pad;
    }

    // Update Alien Presence breathing & globe rotation
    m_alien.breathingTimer += dt * 1.2f;
    m_alien.eyeGlowIntensity = 0.85f + std::sin(m_alien.breathingTimer * 2.0f) * 0.15f;
    m_alien.globeRotation += dt * 0.35f;
}

void CosmicOcean::render(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // Save graphics container/state
    Gdiplus::GraphicsState state = g.Save();
    Gdiplus::RectF clipRect(x, y, w, h);
    g.SetClip(clipRect);

    // 1. Spacetime Deep Void Ocean Background
    renderOceanBackground(g, x, y, w, h);

    // 2. Out-of-the-sea Phenomena (Cosmic Bubbles, Horizon Veil, Topological Vortices)
    renderOutOfSeaPhenomena(g, x, y, w, h);

    // 3. Extraterrestrial Presence (Elder Alien holding Earth in hands)
    renderAlienPresence(g, x, y, w, h);

    // 4. Cosmic Fish swimming through the dark ocean
    for (const auto& fish : m_fish) {
        renderCosmicFish(g, fish);
    }

    // Restore clip
    g.Restore(state);
}

void CosmicOcean::renderOceanBackground(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    // Abyssal gradient for the "Sea of Unobservable Universe"
    Gdiplus::SolidBrush abyssBrush(Gdiplus::Color(255, 6, 8, 16));
    g.FillRectangle(&abyssBrush, x, y, w, h);

    // Fluid wave grid (gravitational wave surface of the sea)
    float cellW = w / static_cast<float>(WAVE_NODES_X - 1);
    float cellH = h / static_cast<float>(WAVE_NODES_Y - 1);

    Gdiplus::Pen wavePen(Gdiplus::Color(35, 30, 90, 160), 1.0f);

    for (int j = 0; j < WAVE_NODES_Y - 1; ++j) {
        for (int i = 0; i < WAVE_NODES_X - 1; ++i) {
            float x1 = x + i * cellW;
            float y1 = y + j * cellH + m_waveHeight[i][j];
            float x2 = x + (i + 1) * cellW;
            float y2 = y + j * cellH + m_waveHeight[i + 1][j];
            float y3 = y + (j + 1) * cellH + m_waveHeight[i][j + 1];

            g.DrawLine(&wavePen, x1, y1, x2, y2);
            g.DrawLine(&wavePen, x1, y1, x1, y3);
        }
    }

    // Glowing cosmic horizon waterline separating Observable bubble from the Infinite Sea
    float horizonY = y + h * 0.16f;
    Gdiplus::SolidBrush waterAura(Gdiplus::Color(35, 0, 200, 255));
    g.FillRectangle(&waterAura, x, horizonY - 12.0f, w, 24.0f);

    Gdiplus::Pen waterLine(Gdiplus::Color(180, 0, 240, 255), 1.8f);
    g.DrawLine(&waterLine, x, horizonY, x + w, horizonY);
}

void CosmicOcean::renderOutOfSeaPhenomena(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    // Entities and phenomena existing outside the cosmic sea:
    // Extra-dimensional brane bubbles, hyper-spacetime rings, vacuum fluctuations
    float t = m_oceanTime;

    // Phenomenon 1: Multiverse Bubble Membrane
    float b1x = x + w * 0.18f + std::sin(t * 0.5f) * 15.0f;
    float b1y = y + h * 0.22f + std::cos(t * 0.4f) * 10.0f;
    Gdiplus::Pen bubblePen(Gdiplus::Color(80, 220, 140, 255), 1.4f);
    g.DrawEllipse(&bubblePen, b1x - 30.0f, b1y - 30.0f, 60.0f, 60.0f);
    
    Gdiplus::SolidBrush bGlow(Gdiplus::Color(20, 220, 140, 255));
    g.FillEllipse(&bGlow, b1x - 30.0f, b1y - 30.0f, 60.0f, 60.0f);

    // Phenomenon 2: Dark Energy Vortex
    float vX = x + w * 0.85f;
    float vY = y + h * 0.28f;
    for (int r = 10; r <= 45; r += 12) {
        float rot = t * 1.5f + r * 0.1f;
        Gdiplus::Pen vortexPen(Gdiplus::Color(65, 255, 100, 80), 1.2f);
        g.DrawArc(&vortexPen, vX - r, vY - r * 0.6f, r * 2.0f, r * 1.2f, rot * 57.3f, 220.0f);
    }
}

void CosmicOcean::renderAlienPresence(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    // Faithfully renders the Elder Alien Presence holding Earth in its hands
    // As in the user's uploaded image: craggy alien cranium, luminous eyes, holding glowing blue/green Earth
    float ax = x + w * 0.50f;
    float ay = y + h * 0.48f;
    float s = (h * 0.32f) / 100.0f;

    float breathY = std::sin(m_alien.breathingTimer) * 3.0f * s;

    // 1. Alien Cranium & Torso Silhouette
    float headW = 55.0f * s;
    float headH = 75.0f * s;
    float hx = ax;
    float hy = ay - 35.0f * s + breathY;

    // Deep craggy alien skull
    Gdiplus::SolidBrush alienSkin(Gdiplus::Color(255, 45, 55, 68)); // Slate gray-blue textured alien skin
    g.FillEllipse(&alienSkin, hx - headW * 0.5f, hy - headH * 0.5f, headW, headH);

    // Cranial ridges / wrinkles (matching image)
    Gdiplus::Pen wrinklePen(Gdiplus::Color(180, 25, 32, 42), 1.5f * s);
    for (float ry = -25.0f; ry <= 10.0f; ry += 6.0f) {
        g.DrawArc(&wrinklePen, hx - headW * 0.38f, hy + ry * s, headW * 0.76f, 12.0f * s, 10.0f, 160.0f);
    }

    // Piercing Alien Eyes (Luminous Cyan/Emerald as in image)
    float eyeY = hy + 4.0f * s;
    float eyeSpacing = 16.0f * s;
    BYTE eyeAlpha = static_cast<BYTE>(clampf(m_alien.eyeGlowIntensity * 255.0f, 150.0f, 255.0f));
    
    // Left eye
    Gdiplus::SolidBrush eyeAura(Gdiplus::Color(100, 0, 255, 200));
    g.FillEllipse(&eyeAura, hx - eyeSpacing - 7.0f * s, eyeY - 4.0f * s, 14.0f * s, 8.0f * s);
    Gdiplus::SolidBrush eyeCore(Gdiplus::Color(eyeAlpha, 220, 255, 245));
    g.FillEllipse(&eyeCore, hx - eyeSpacing - 4.0f * s, eyeY - 2.5f * s, 8.0f * s, 5.0f * s);

    // Right eye
    g.FillEllipse(&eyeAura, hx + eyeSpacing - 7.0f * s, eyeY - 4.0f * s, 14.0f * s, 8.0f * s);
    g.FillEllipse(&eyeCore, hx + eyeSpacing - 4.0f * s, eyeY - 2.5f * s, 8.0f * s, 5.0f * s);

    // Elongated Alien Shoulders & Arms cradling Earth
    Gdiplus::Pen armPen(Gdiplus::Color(255, 50, 60, 75), 7.0f * s);
    armPen.SetStartCap(Gdiplus::LineCapRound);
    armPen.SetEndCap(Gdiplus::LineCapRound);

    // Left arm wrapping around globe
    g.DrawArc(&armPen, ax - 55.0f * s, ay + 5.0f * s, 50.0f * s, 50.0f * s, 80.0f, 140.0f);
    // Right arm wrapping around globe
    g.DrawArc(&armPen, ax + 5.0f * s, ay + 5.0f * s, 50.0f * s, 50.0f * s, 320.0f, 140.0f);

    // Slender fingers cradling the planet
    Gdiplus::Pen fingerPen(Gdiplus::Color(255, 65, 78, 95), 2.8f * s);
    for (int f = 0; f < 4; ++f) {
        float fOffset = f * 5.0f * s;
        // Left fingers
        g.DrawLine(&fingerPen, ax - 32.0f * s + fOffset, ay + 42.0f * s, ax - 24.0f * s + fOffset, ay + 48.0f * s);
        // Right fingers
        g.DrawLine(&fingerPen, ax + 32.0f * s - fOffset, ay + 42.0f * s, ax + 24.0f * s - fOffset, ay + 48.0f * s);
    }

    // 2. Planet Earth in Alien's Hands (Glowing blue oceans, green continents, atmospheric haze)
    renderAlienEarth(g, ax, ay + 38.0f * s, 26.0f * s);

    // 3. Computer Vision (CV) Bounding Box and Telemetry tracking the Alien Presence
    renderCVOverlay(g, ax - 65.0f * s, hy - 45.0f * s, 130.0f * s, 125.0f * s, L"CLASS-IV ELDER OBSERVER [EARTH GUARDIAN]", m_alien.cvConfidence);
}

void CosmicOcean::renderAlienEarth(Gdiplus::Graphics& g, float ex, float ey, float radius) {
    // Planet Earth globe
    Gdiplus::GraphicsPath earthPath;
    earthPath.AddEllipse(ex - radius, ey - radius, radius * 2.0f, radius * 2.0f);

    // Deep blue ocean sphere
    Gdiplus::SolidBrush oceanBrush(Gdiplus::Color(255, 15, 65, 145));
    g.FillPath(&oceanBrush, &earthPath);

    // Green continents rotating
    Gdiplus::SolidBrush continentBrush(Gdiplus::Color(255, 45, 140, 65));
    float rotOffset = std::fmod(m_alien.globeRotation * radius * 0.8f, radius * 2.0f);

    // Continent silhouettes
    g.FillEllipse(&continentBrush, ex - radius * 0.6f + rotOffset - radius * 0.5f, ey - radius * 0.4f, radius * 0.7f, radius * 0.6f);
    g.FillEllipse(&continentBrush, ex - radius * 0.3f + rotOffset - radius * 0.5f, ey + radius * 0.1f, radius * 0.6f, radius * 0.5f);

    // Atmospheric luminous blue glow rim
    Gdiplus::Pen atmoPen(Gdiplus::Color(180, 80, 200, 255), 2.2f);
    g.DrawEllipse(&atmoPen, ex - radius, ey - radius, radius * 2.0f, radius * 2.0f);

    Gdiplus::SolidBrush glintBrush(Gdiplus::Color(100, 255, 255, 255));
    g.FillEllipse(&glintBrush, ex - radius * 0.6f, ey - radius * 0.6f, radius * 0.5f, radius * 0.3f);
}

void CosmicOcean::renderCVOverlay(Gdiplus::Graphics& g, float bx, float by, float bw, float bh, const wchar_t* label, float conf) {
    // High-tech Computer Vision corner brackets (inspired by user instructions)
    Gdiplus::Pen cvPen(Gdiplus::Color(220, 0, 255, 180), 1.5f);
    float cornerLen = 14.0f;

    // Top-Left
    g.DrawLine(&cvPen, bx, by, bx + cornerLen, by);
    g.DrawLine(&cvPen, bx, by, bx, by + cornerLen);
    // Top-Right
    g.DrawLine(&cvPen, bx + bw, by, bx + bw - cornerLen, by);
    g.DrawLine(&cvPen, bx + bw, by, bx + bw, by + cornerLen);
    // Bottom-Left
    g.DrawLine(&cvPen, bx, by + bh, bx + cornerLen, by + bh);
    g.DrawLine(&cvPen, bx, by + bh, bx, by + bh - cornerLen);
    // Bottom-Right
    g.DrawLine(&cvPen, bx + bw, by + bh, bx + bw - cornerLen, by + bh);
    g.DrawLine(&cvPen, bx + bw, by + bh, bx + bw, by + bh - cornerLen);

    // Crosshair in center
    float cx = bx + bw * 0.5f;
    float cy = by + bh * 0.5f;
    g.DrawLine(&cvPen, cx - 6.0f, cy, cx + 6.0f, cy);
    g.DrawLine(&cvPen, cx, cy - 6.0f, cx, cy + 6.0f);

    // Label tag
    Gdiplus::Font tagFont(L"Consolas", 7.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush tagBg(Gdiplus::Color(190, 10, 24, 34));
    g.FillRectangle(&tagBg, bx, by - 14.0f, bw, 13.0f);

    Gdiplus::SolidBrush tagText(Gdiplus::Color(255, 0, 255, 180));
    wchar_t buf[128];
    swprintf_s(buf, L"[CV LOCK %.1f%%] %s", conf * 100.0f, label);
    g.DrawString(buf, -1, &tagFont, Gdiplus::PointF(bx + 2.0f, by - 13.0f), &tagText);
}

void CosmicOcean::renderCosmicFish(Gdiplus::Graphics& g, const CosmicFish& fish) {
    float fx = fish.position.x;
    float fy = fish.position.y;
    float s = fish.size / 30.0f;

    // Save transform
    Gdiplus::Matrix mat;
    mat.RotateAt(fish.headingAngle * 57.2958f, Gdiplus::PointF(fx, fy));
    g.SetTransform(&mat);

    if (fish.type == CREATURE_VACUUM_MANTA) {
        // Undulating manta wings
        float wingSpan = 28.0f * s;
        float bodyLen = 32.0f * s;
        float flap = std::sin(fish.finOscillation) * 8.0f * s;

        Gdiplus::GraphicsPath mantaPath;
        Gdiplus::PointF pts[5] = {
            Gdiplus::PointF(fx + bodyLen * 0.5f, fy),
            Gdiplus::PointF(fx - bodyLen * 0.2f, fy - wingSpan + flap),
            Gdiplus::PointF(fx - bodyLen * 0.5f, fy),
            Gdiplus::PointF(fx - bodyLen * 0.2f, fy + wingSpan - flap),
            Gdiplus::PointF(fx + bodyLen * 0.5f, fy)
        };
        mantaPath.AddLines(pts, 5);

        // Bioluminescent fill
        Gdiplus::SolidBrush mantaBrush(fish.glowColor);
        g.FillPath(&mantaBrush, &mantaPath);

        // Trailing luminous whip tail
        Gdiplus::Pen tailPen(fish.glowColor, 1.5f);
        g.DrawLine(&tailPen, fx - bodyLen * 0.5f, fy, fx - bodyLen * 1.3f, fy + flap * 0.8f);

    } else if (fish.type == CREATURE_GRAVITON_EEL) {
        // Serpentine sinuous eel
        int segments = 8;
        float segLen = 5.0f * s;
        Gdiplus::Pen eelPen(fish.glowColor, 3.5f * s);

        for (int i = 0; i < segments - 1; ++i) {
            float sx1 = fx - i * segLen;
            float sy1 = fy + std::sin(fish.finOscillation + i * 0.7f) * 6.0f * s;
            float sx2 = fx - (i + 1) * segLen;
            float sy2 = fy + std::sin(fish.finOscillation + (i + 1) * 0.7f) * 6.0f * s;
            g.DrawLine(&eelPen, sx1, sy1, sx2, sy2);
        }

        // Glowing eel head
        Gdiplus::SolidBrush headBrush(Gdiplus::Color(255, 255, 255, 255));
        g.FillEllipse(&headBrush, fx - 3.0f * s, fy - 3.0f * s, 6.0f * s, 6.0f * s);

    } else {
        // Tachyon Siphonophore Colony (cluster of glowing bells and trailing tentacles)
        float bellR = 8.0f * s;
        Gdiplus::SolidBrush bellBrush(fish.glowColor);
        g.FillEllipse(&bellBrush, fx - bellR, fy - bellR * 0.7f, bellR * 2.0f, bellR * 1.4f);

        Gdiplus::Pen tentaclePen(fish.glowColor, 1.0f);
        for (int t = 0; t < 5; ++t) {
            float tx = fx - bellR + t * 4.0f * s;
            float ty = fy + std::sin(fish.finOscillation + t * 0.8f) * 8.0f * s;
            g.DrawLine(&tentaclePen, tx, fy, tx - 16.0f * s, ty);
        }
    }

    // Reset transform
    g.ResetTransform();

    // CV tag tracking the fish
    Gdiplus::Font fishFont(L"Consolas", 6.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush fishTagBrush(fish.glowColor);
    g.DrawString(fish.speciesName.c_str(), -1, &fishFont, Gdiplus::PointF(fx + 12.0f, fy - 8.0f), &fishTagBrush);
}

} // namespace CosmicRobotics
