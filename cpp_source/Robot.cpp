#include "Robot.hpp"
#include <cstdio>

namespace CosmicRobotics {

Robot::Robot(RobotID id, const std::wstring& name, const std::wstring& title, const std::wstring& roleDescription)
    : m_id(id)
    , m_name(name)
    , m_title(title)
    , m_roleDescription(roleDescription)
    , m_isSpeaking(false)
    , m_speechAnimTimer(0.0f)
    , m_statusBlink(0.0f)
    , m_themeColor(Gdiplus::Color(255, 0, 220, 255))
{
    m_face = std::unique_ptr<FaceModel>(new FaceModel(id));
}

Robot::~Robot() {}

void Robot::setSpeaking(bool speaking, Viseme viseme) {
    m_isSpeaking = speaking;
    if (m_isSpeaking) {
        m_face->setViseme(viseme);
        if (m_face->getEmotion() == EMOTION_NEUTRAL) {
            m_face->setEmotion(EMOTION_SPEAKING);
        }
    } else {
        m_face->setViseme(VISEME_REST);
    }
}

void Robot::setEmotion(Emotion emotion) {
    m_face->setEmotion(emotion);
}

void Robot::setGaze(float gx, float gy) {
    m_face->setGazeTarget(gx, gy);
}

void Robot::update(float dt) {
    m_face->update(dt);
    m_speechAnimTimer += dt;
    m_statusBlink += dt * 3.0f;
}

void Robot::renderCard(Gdiplus::Graphics& g, float x, float y, float w, float h, bool isHighlighted) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // Card background
    Gdiplus::Color bgColor = isHighlighted ? Gdiplus::Color(255, 18, 25, 42) : Gdiplus::Color(255, 12, 16, 26);
    Gdiplus::SolidBrush bgBrush(bgColor);
    g.FillRectangle(&bgBrush, x, y, w, h);

    // Border
    Gdiplus::Color bColor = isHighlighted ? m_themeColor : Gdiplus::Color(255, 40, 55, 75);
    Gdiplus::Pen borderPen(bColor, isHighlighted ? 2.0f : 1.2f);
    g.DrawRectangle(&borderPen, x, y, w, h);

    // Header badge
    Gdiplus::SolidBrush headerBg(Gdiplus::Color(255, 18, 24, 38));
    g.FillRectangle(&headerBg, x, y, w, 24.0f);

    Gdiplus::Font titleFont(L"Consolas", 8.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush textBrush(m_themeColor);
    g.DrawString(m_name.c_str(), -1, &titleFont, Gdiplus::PointF(x + 8.0f, y + 4.0f), &textBrush);

    // Live speaking LED indicator
    float ledX = x + w - 16.0f;
    float ledY = y + 12.0f;
    if (m_isSpeaking) {
        BYTE ledAlpha = static_cast<BYTE>(std::sin(m_statusBlink * 3.0f) * 80.0f + 175.0f);
        Gdiplus::SolidBrush ledGlow(Gdiplus::Color(ledAlpha, 0, 255, 180));
        g.FillEllipse(&ledGlow, ledX - 5.0f, ledY - 5.0f, 10.0f, 10.0f);
        
        Gdiplus::SolidBrush ledCore(Gdiplus::Color(255, 255, 255, 255));
        g.FillEllipse(&ledCore, ledX - 2.5f, ledY - 2.5f, 5.0f, 5.0f);
    } else {
        Gdiplus::SolidBrush ledOff(Gdiplus::Color(255, 50, 70, 80));
        g.FillEllipse(&ledOff, ledX - 3.5f, ledY - 3.5f, 7.0f, 7.0f);
    }

    // Render Robot's Human-like Face in center of card
    float faceCenterX = x + w * 0.5f;
    float faceCenterY = y + h * 0.52f;
    float faceScale = (h * 0.36f) / 100.0f;
    m_face->render(g, faceCenterX, faceCenterY, faceScale);

    // Footer info / role description
    Gdiplus::Font descFont(L"Consolas", 6.8f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush descBrush(Gdiplus::Color(255, 170, 190, 210));
    g.DrawString(m_title.c_str(), -1, &descFont, Gdiplus::PointF(x + 8.0f, y + h - 20.0f), &descBrush);

    // Audio frequency visualizer bars when speaking
    if (m_isSpeaking) {
        float barStartX = x + w - 45.0f;
        float barY = y + h - 12.0f;
        for (int b = 0; b < 5; ++b) {
            float barH = (std::sin(m_speechAnimTimer * 12.0f + b * 1.2f) * 0.5f + 0.5f) * 10.0f + 2.0f;
            Gdiplus::SolidBrush barBrush(m_themeColor);
            g.FillRectangle(&barBrush, barStartX + b * 6.0f, barY - barH, 4.0f, barH);
        }
    }
}

// -------------------------------------------------------------
// ROBOT CORNER: "EVA-01" (Frontline Liaison speaking to user at laptop)
// -------------------------------------------------------------
RobotCorner::RobotCorner()
    : Robot(ROBOT_CORNER, L"EVA-01 :: CORNER LIAISON", L"Direct Interface to User at Laptop", L"User Co-Pilot & Cosmic Liaison")
{
    m_themeColor = Gdiplus::Color(255, 0, 240, 255); // Radiant Cyan
}

std::wstring RobotCorner::getIntroSpeech() {
    return L"Greetings, Commander. I am EVA, speaking directly to you here at your laptop. We are guarding the simulation of the unobservable universe. To our right lies the vast cosmic ocean. Chronos-Void is streaming every byte of data collected from the void, while Thalassa monitors the exotic fish and extraterrestrial presences swimming in that abyssal sea.";
}

std::wstring RobotCorner::getRandomDialogue() {
    static const wchar_t* LINES[] = {
        L"I am tracking your gaze from your laptop screen. Our sensor telemetry across the cosmic sea is green.",
        L"Look at the right screen: our satellite dish is intercepting high-gain quantum signals from the unobservable sea.",
        L"Notice the alien entity identified by our computer vision filters. Ancient intelligences watch over planetary spheres across the void.",
        L"The unobservable universe behaves like a colossal cosmic ocean. Light cannot escape its event horizon, but quantum carrier beams penetrate.",
        L"I have calibrated our odometer and speed instruments. We are synchronized with the cosmic expansion rate."
    };
    return LINES[rand() % 5];
}

std::wstring RobotCorner::getCosmicOceanReport() {
    return L"Commander, our corner terminal is receiving unprecedented signals from the unobservable ocean. Sensor arrays detect large-scale extraterrestrial presences beyond our forty-six billion light-year bubble. Our squad is holding steady surveillance.";
}

void RobotCorner::renderLaptopWorkstation(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // Laptop Base / Keyboard deck
    float lapW = w * 0.74f;
    float lapH = 34.0f;
    float lapX = x + (w - lapW) * 0.5f;
    float lapY = y + h - lapH - 10.0f;

    Gdiplus::SolidBrush baseBrush(Gdiplus::Color(255, 30, 35, 48));
    g.FillRectangle(&baseBrush, lapX, lapY, lapW, lapH);

    Gdiplus::Pen baseEdgePen(Gdiplus::Color(255, 80, 110, 140), 1.5f);
    g.DrawRectangle(&baseEdgePen, lapX, lapY, lapW, lapH);

    // Trackpad
    Gdiplus::SolidBrush padBrush(Gdiplus::Color(255, 45, 52, 68));
    g.FillRectangle(&padBrush, lapX + lapW * 0.38f, lapY + 10.0f, lapW * 0.24f, 18.0f);

    // Keyboard backlighting glow
    Gdiplus::SolidBrush kbGlow(Gdiplus::Color(120, 0, 230, 255));
    g.FillRectangle(&kbGlow, lapX + lapW * 0.12f, lapY + 3.0f, lapW * 0.76f, 5.0f);

    // Laptop Screen (angled upward toward user)
    float scrW = lapW * 0.88f;
    float scrH = h * 0.56f;
    float scrX = lapX + (lapW - scrW) * 0.5f;
    float scrY = lapY - scrH + 6.0f;

    Gdiplus::SolidBrush scrBezel(Gdiplus::Color(255, 20, 24, 32));
    g.FillRectangle(&scrBezel, scrX, scrY, scrW, scrH);
    g.DrawRectangle(&baseEdgePen, scrX, scrY, scrW, scrH);

    Gdiplus::SolidBrush displayGlass(Gdiplus::Color(255, 10, 18, 30));
    g.FillRectangle(&displayGlass, scrX + 6.0f, scrY + 6.0f, scrW - 12.0f, scrH - 12.0f);

    // Webcam making eye contact with user
    Gdiplus::SolidBrush camBrush(Gdiplus::Color(255, 0, 255, 200));
    g.FillEllipse(&camBrush, scrX + scrW * 0.5f - 2.5f, scrY + 2.0f, 5.0f, 5.0f);

    // Live terminal text on user's laptop screen
    Gdiplus::Font scrFont(L"Consolas", 6.8f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush termText(Gdiplus::Color(255, 0, 240, 255));
    g.DrawString(L"USER TERMINAL :: EVA-01 LINK", -1, &scrFont, Gdiplus::PointF(scrX + 8.0f, scrY + 8.0f), &termText);
    
    Gdiplus::SolidBrush subText(Gdiplus::Color(255, 160, 210, 240));
    g.DrawString(L"> COSMIC SEA SIMULATOR: GUARDED", -1, &scrFont, Gdiplus::PointF(scrX + 8.0f, scrY + 20.0f), &subText);
    g.DrawString(L"> SATELLITE CARRIER: 248.8 GHz", -1, &scrFont, Gdiplus::PointF(scrX + 8.0f, scrY + 32.0f), &subText);
    g.DrawString(L"> USER AT LAPTOP: SYNCHRONIZED", -1, &scrFont, Gdiplus::PointF(scrX + 8.0f, scrY + 44.0f), &subText);
}

// -------------------------------------------------------------
// ROBOT UNOBSERVABLE: "CHRONOS-VOID" (Talking about EVERY DATA collected from unobservable universe)
// -------------------------------------------------------------
RobotUnobservable::RobotUnobservable()
    : Robot(ROBOT_UNOBSERVABLE, L"CHRONOS-VOID :: DATA NEXUS", L"Unobservable Universe Data Collector", L"Relays All Cosmic Void Telemetry")
{
    m_themeColor = Gdiplus::Color(255, 190, 80, 255); // Deep Cosmic Violet/Gold
}

std::wstring RobotUnobservable::getIntroSpeech() {
    return L"This is Chronos-Void, broadcasting all data collected from the unobservable universe. Telemetry logs confirm dark flow streaming at eight hundred kilometers per second. Metric expansion z-index exceeds eleven hundred. Vacuum curvature tensor indicates infinite non-baryonic matter reservoirs outside your observable bubble.";
}

std::wstring RobotUnobservable::getRandomDialogue() {
    static const wchar_t* LINES[] = {
        L"Every data packet collected from the unobservable sea is being beamed directly to your observatory receiver.",
        L"Telemetry update: Cosmic microwave dipole drift recorded at six hundred twenty-seven kilometers per second.",
        L"Data register seventy-four: Non-baryonic dark matter density gradients indicate super-horizon topological defects.",
        L"Quantum entanglement coherence remains at ninety-nine point eight percent across the event horizon boundary.",
        L"Deep space telemetry confirms: The unobservable realm is infinite in extent, housing untold stellar structures."
    };
    return LINES[rand() % 5];
}

std::wstring RobotUnobservable::getCosmicOceanReport() {
    return L"Comprehensive data dump from the unobservable sea: Cosmic microwave background temperature plummets to micro-Kelvin levels. Pre-inflationary gravitational wave ripples are detected, carrying signatures of cosmological structures millions of times larger than the observable sphere.";
}

// -------------------------------------------------------------
// ROBOT ODOMETER / XENOBIOLOGIST: "THALASSA-BIO" (Speaking about KIND OF FISH in the cosmic sea & aliens)
// -------------------------------------------------------------
RobotOdometer::RobotOdometer()
    : Robot(ROBOT_ODOMETER, L"THALASSA-BIO :: SEA OF VOID", L"Cosmic Ocean Biota & Alien Guardian", L"Surveys Fish in the Sea & Alien Presences")
{
    m_themeColor = Gdiplus::Color(255, 30, 210, 180); // Bioluminescent Sea Cyan/Green
}

std::wstring RobotOdometer::getIntroSpeech() {
    return L"I am Thalassa, guardian of the cosmic ocean. In this sea of the unobservable universe, we observe exotic forms of cosmic life swimming through the gravitational ether. We have cataloged bioluminescent vacuum mantas, tachyon siphonophores, and ancient alien observers that hold entire planetary spheres in their hands.";
}

std::wstring RobotOdometer::getRandomDialogue() {
    static const wchar_t* LINES[] = {
        L"Look into the cosmic sea on the right. Notice the abyssal photonic manta undulating through dark matter currents.",
        L"Alien intelligence detected: A colossal observer entity has been mapped by our computer vision filters, guarding planetary worlds.",
        L"The fish of this unobservable ocean are non-baryonic. They feed on zero-point energy fluctuations and gravitational wave ripples.",
        L"We have cataloged three distinct cosmic marine species: Vacuum Siphonophores, Graviton Eels, and Abyssal Cetaceans.",
        L"Spacetime odometer registers deep ocean immersion: Forty-six point eight billion light-years deep into the cosmic sea."
    };
    return LINES[rand() % 5];
}

std::wstring RobotOdometer::getCosmicOceanReport() {
    return L"Xenobiology report from the unobservable sea: The alien presence shown on our optical scan is an Elder Cosmological Guardian. Just as marine predators protect coral atolls, these colossal intelligences harbor fragile infant worlds like Earth within their cosmic palm.";
}

// -------------------------------------------------------------
// ROBOT SPEED: "VELOX-TACHYON" (Relativistic Velocity & Odometer Specialist)
// -------------------------------------------------------------
RobotSpeed::RobotSpeed()
    : Robot(ROBOT_SPEED, L"VELOX-TACHYON :: SPEED MONITOR", L"Relativistic Kinematics & Odometer", L"Tachometer, Odometer & Metric Warp")
{
    m_themeColor = Gdiplus::Color(255, 255, 140, 20); // Amber Gold
}

std::wstring RobotSpeed::getIntroSpeech() {
    return L"Velox-Tachyon reporting! Tachometer and distance odometer are active. As we dive through the unobservable cosmic ocean, relativistic velocity is locked at zero point nine nine four c. Lorentz dilation factor gamma is at nine point one four. Ready for warp pulses through the cosmic currents!";
}

std::wstring RobotSpeed::getRandomDialogue() {
    static const wchar_t* LINES[] = {
        L"Velocity surge detected through the cosmic ocean current! Instantaneous speed registered at zero point nine nine four c.",
        L"Distance odometer advancing through the unobservable sea. We are logging forty-seven Giga-light-years and climbing.",
        L"Doppler blueshift visible on our forward sensors as we surf the gravitational waves of the unobservable void.",
        L"Engaging metric warp pulse! Apparent velocity expanding faster than photon propagation."
    };
    return LINES[rand() % 4];
}

std::wstring RobotSpeed::getCosmicOceanReport() {
    return L"Kinematics report: Navigating the unobservable ocean requires compensations for metric superluminal expansion. Proper distance is accumulating at fifty-eight megaparsecs per simulated cycle. Time dilation factor gamma holds stable at nine point one four.";
}

} // namespace CosmicRobotics
