#include "FaceModel.hpp"

namespace CosmicRobotics {

FaceModel::FaceModel(RobotID robotId)
    : m_robotId(robotId)
    , m_targetEmotion(EMOTION_NEUTRAL)
    , m_currentViseme(VISEME_REST)
    , m_blinkTimer(0.0f)
    , m_nextBlinkInterval(3.5f)
    , m_isBlinking(false)
    , m_blinkProgress(0.0f)
    , m_timeElapsed(0.0f)
    , m_saccadeTimer(0.0f)
    , m_saccadeOffset(0.0f, 0.0f)
    , m_gazeTarget(0.0f, 0.0f)
{
    // Configure aesthetics based on robot identity and uploaded reference designs
    switch (m_robotId) {
        case ROBOT_CORNER:
            // Porcelain synthetic skin with warm tones, rose lips, luminous cyan iris
            m_skinBaseColor = Gdiplus::Color(255, 244, 236, 228);
            m_skinHighlightColor = Gdiplus::Color(255, 255, 250, 245);
            m_irisGlowColor = Gdiplus::Color(255, 0, 225, 255);       // Brilliant cyan
            m_plateAccentColor = Gdiplus::Color(255, 225, 230, 240); // Pearl white titanium
            m_lipColor = Gdiplus::Color(255, 215, 125, 130);         // Soft human rose
            break;

        case ROBOT_UNOBSERVABLE:
            // Obsidian & deep cosmic void styling with glowing violet/gold quantum optics
            m_skinBaseColor = Gdiplus::Color(255, 45, 40, 55);
            m_skinHighlightColor = Gdiplus::Color(255, 75, 70, 95);
            m_irisGlowColor = Gdiplus::Color(255, 190, 80, 255);     // Deep cosmic purple
            m_plateAccentColor = Gdiplus::Color(255, 30, 25, 40);    // Deep obsidian
            m_lipColor = Gdiplus::Color(255, 110, 80, 130);
            break;

        case ROBOT_ODOMETER:
            // Precision silver-cobalt surveyor styling with sapphire analytical optics
            m_skinBaseColor = Gdiplus::Color(255, 220, 230, 242);
            m_skinHighlightColor = Gdiplus::Color(255, 245, 250, 255);
            m_irisGlowColor = Gdiplus::Color(255, 30, 144, 255);     // Sapphire blue
            m_plateAccentColor = Gdiplus::Color(255, 180, 195, 215); // Metallic silver
            m_lipColor = Gdiplus::Color(255, 175, 130, 140);
            break;

        case ROBOT_SPEED:
            // High-velocity aerodynamic styling with fiery amber-gold luminous optics
            m_skinBaseColor = Gdiplus::Color(255, 235, 235, 240);
            m_skinHighlightColor = Gdiplus::Color(255, 255, 255, 255);
            m_irisGlowColor = Gdiplus::Color(255, 255, 160, 20);     // Amber gold
            m_plateAccentColor = Gdiplus::Color(255, 200, 50, 40);   // Crimson & gold trim
            m_lipColor = Gdiplus::Color(255, 190, 110, 100);
            break;
    }

    m_targetParams = getEmotionTargetParams(EMOTION_NEUTRAL);
    m_currentParams = m_targetParams;
}

FaceModel::~FaceModel() {}

void FaceModel::setEmotion(Emotion emotion) {
    m_targetEmotion = emotion;
    m_targetParams = getEmotionTargetParams(emotion);
}

void FaceModel::setViseme(Viseme viseme) {
    m_currentViseme = viseme;
}

void FaceModel::setGazeTarget(float targetX, float targetY) {
    m_gazeTarget.x = clampf(targetX, -1.0f, 1.0f);
    m_gazeTarget.y = clampf(targetY, -1.0f, 1.0f);
}

void FaceModel::triggerBlink() {
    m_isBlinking = true;
    m_blinkProgress = 0.0f;
}

FaceParameters FaceModel::getEmotionTargetParams(Emotion emotion) const {
    FaceParameters p;
    switch (emotion) {
        case EMOTION_NEUTRAL:
            p.browInnerY = 0.0f;
            p.browOuterY = 0.0f;
            p.browArch = 0.45f;
            p.mouthSmile = 0.15f;
            p.eyeOpenness = 1.0f;
            p.pupilDilation = 1.0f;
            break;

        case EMOTION_SPEAKING:
            p.browInnerY = 0.15f;
            p.browOuterY = 0.05f;
            p.browArch = 0.55f;
            p.mouthSmile = 0.35f;
            p.eyeOpenness = 1.05f;
            p.pupilDilation = 1.1f;
            break;

        case EMOTION_HAPPY:
            p.browInnerY = 0.25f;
            p.browOuterY = 0.2f;
            p.browArch = 0.65f;
            p.mouthSmile = 0.75f;
            p.eyeOpenness = 0.95f; // Slight squint of joy
            p.pupilDilation = 1.15f;
            break;

        case EMOTION_THOUGHTFUL:
            p.browInnerY = -0.2f;
            p.browOuterY = 0.25f;
            p.browArch = 0.6f;
            p.mouthSmile = -0.05f;
            p.headRoll = 0.08f;
            p.eyeOpenness = 0.9f;
            p.pupilDilation = 0.95f;
            break;

        case EMOTION_SURPRISED:
            p.browInnerY = 0.65f;
            p.browOuterY = 0.6f;
            p.browArch = 0.85f;
            p.mouthSmile = 0.1f;
            p.eyeOpenness = 1.25f; // Wide open
            p.pupilDilation = 1.35f;
            break;

        case EMOTION_ANALYTICAL:
            p.browInnerY = -0.35f; // Furrowed brow
            p.browOuterY = -0.1f;
            p.browArch = 0.35f;
            p.mouthSmile = -0.1f;
            p.eyeOpenness = 0.92f;
            p.pupilDilation = 0.88f;
            break;

        case EMOTION_AWE:
            p.browInnerY = 0.45f;
            p.browOuterY = 0.35f;
            p.browArch = 0.75f;
            p.mouthSmile = 0.4f;
            p.eyeOpenness = 1.2f;
            p.pupilDilation = 1.3f;
            break;
    }
    return p;
}

void FaceModel::applyVisemeToParams(Viseme viseme, FaceParameters& params) const {
    switch (viseme) {
        case VISEME_REST:
            params.mouthOpenness = 0.0f;
            params.mouthWidth = 1.0f;
            params.lipSeparation = 0.0f;
            break;
        case VISEME_AA: // "Ah", "Ha"
            params.mouthOpenness = 0.65f;
            params.mouthWidth = 1.05f;
            params.lipSeparation = 0.5f;
            break;
        case VISEME_EE: // "Ee", "See"
            params.mouthOpenness = 0.35f;
            params.mouthWidth = 1.35f;
            params.lipSeparation = 0.25f;
            break;
        case VISEME_OH: // "Oh", "You"
            params.mouthOpenness = 0.55f;
            params.mouthWidth = 0.65f;
            params.lipSeparation = 0.4f;
            break;
        case VISEME_MM: // "Mmm", "B"
            params.mouthOpenness = 0.05f;
            params.mouthWidth = 0.95f;
            params.lipSeparation = 0.0f;
            break;
    }
}

void FaceModel::updateBlinking(float dt) {
    if (!m_isBlinking) {
        m_blinkTimer += dt;
        if (m_blinkTimer >= m_nextBlinkInterval) {
            triggerBlink();
            m_blinkTimer = 0.0f;
            m_nextBlinkInterval = randFloat(2.5f, 5.0f);
        }
    } else {
        // Fast closing (60ms), realistic spring-back opening (140ms) -> total ~200ms
        const float blinkDuration = 0.20f;
        m_blinkProgress += dt / blinkDuration;
        if (m_blinkProgress >= 1.0f) {
            m_isBlinking = false;
            m_blinkProgress = 0.0f;
        }
    }
}

void FaceModel::updateMicroMovements(float dt) {
    m_timeElapsed += dt;
    
    // Natural micro-saccades (subtle eye twitches that create genuine lifelike presence)
    m_saccadeTimer += dt;
    if (m_saccadeTimer > randFloat(1.2f, 2.8f)) {
        m_saccadeTimer = 0.0f;
        m_saccadeOffset.x = randFloat(-0.04f, 0.04f);
        m_saccadeOffset.y = randFloat(-0.03f, 0.03f);
    }

    // Natural breathing cycle and subtle head micro-sway
    float breath = std::sin(m_timeElapsed * 1.5f) * 0.015f;
    float swayYaw = std::sin(m_timeElapsed * 0.8f) * 0.02f;
    
    m_currentParams.headPitch += breath;
    m_currentParams.headYaw += swayYaw;
}

void FaceModel::interpolateParameters(float dt) {
    FaceParameters goal = m_targetParams;
    applyVisemeToParams(m_currentViseme, goal);

    // Dynamic gaze target combined with saccades
    goal.eyeGazeX = m_gazeTarget.x + m_saccadeOffset.x;
    goal.eyeGazeY = m_gazeTarget.y + m_saccadeOffset.y;

    // Blink modifier
    float blinkFactor = 1.0f;
    if (m_isBlinking) {
        if (m_blinkProgress < 0.4f) {
            // Rapid closure
            blinkFactor = 1.0f - (m_blinkProgress / 0.4f);
        } else {
            // Smooth rebound
            blinkFactor = (m_blinkProgress - 0.4f) / 0.6f;
        }
    }
    goal.eyeOpenness *= blinkFactor;

    // Smooth exponential decay interpolation
    const float blendRate = 12.0f;
    float t = clampf(dt * blendRate, 0.0f, 1.0f);

    m_currentParams.eyeOpenness   = lerpf(m_currentParams.eyeOpenness, goal.eyeOpenness, t);
    m_currentParams.eyeGazeX      = lerpf(m_currentParams.eyeGazeX, goal.eyeGazeX, t * 1.5f);
    m_currentParams.eyeGazeY      = lerpf(m_currentParams.eyeGazeY, goal.eyeGazeY, t * 1.5f);
    m_currentParams.pupilDilation = lerpf(m_currentParams.pupilDilation, goal.pupilDilation, t);
    m_currentParams.browInnerY    = lerpf(m_currentParams.browInnerY, goal.browInnerY, t);
    m_currentParams.browOuterY    = lerpf(m_currentParams.browOuterY, goal.browOuterY, t);
    m_currentParams.browArch      = lerpf(m_currentParams.browArch, goal.browArch, t);
    m_currentParams.mouthOpenness = lerpf(m_currentParams.mouthOpenness, goal.mouthOpenness, t * 1.8f);
    m_currentParams.mouthWidth    = lerpf(m_currentParams.mouthWidth, goal.mouthWidth, t * 1.5f);
    m_currentParams.mouthSmile    = lerpf(m_currentParams.mouthSmile, goal.mouthSmile, t);
    m_currentParams.lipSeparation = lerpf(m_currentParams.lipSeparation, goal.lipSeparation, t * 1.8f);
}

void FaceModel::update(float dt) {
    updateBlinking(dt);
    updateMicroMovements(dt);
    interpolateParameters(dt);
}

void FaceModel::render(Gdiplus::Graphics& g, float centerX, float centerY, float scale) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // Subtle 3D head offset based on head yaw/pitch
    float hx = centerX + m_currentParams.headYaw * 20.0f * scale;
    float hy = centerY + m_currentParams.headPitch * 15.0f * scale;

    drawHeadBase(g, hx, hy, scale);
    drawCyberneticPlates(g, hx, hy, scale);
    drawNose(g, hx, hy, scale);
    drawEyes(g, hx, hy, scale);
    drawEyebrows(g, hx, hy, scale);
    drawMouth(g, hx, hy, scale);
    drawHUDSensors(g, hx, hy, scale);
}

void FaceModel::drawHeadBase(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Elegant humanoid contoured head: cranial dome, jawline tapering to feminine/sleek chin
    float headW = 120.0f * s;
    float headH = 155.0f * s;

    // Cybernetic neck
    Gdiplus::SolidBrush neckBrush(Gdiplus::Color(255, 40, 44, 52));
    g.FillRectangle(&neckBrush, cx - 24.0f * s, cy + 50.0f * s, 48.0f * s, 45.0f * s);
    
    // Neck hydraulics / vertebrae
    Gdiplus::Pen neckPistons(Gdiplus::Color(255, 120, 130, 150), 2.0f * s);
    g.DrawLine(&neckPistons, cx - 14.0f * s, cy + 52.0f * s, cx - 14.0f * s, cy + 90.0f * s);
    g.DrawLine(&neckPistons, cx + 14.0f * s, cy + 52.0f * s, cx + 14.0f * s, cy + 90.0f * s);

    // Cranium / Face Oval
    Gdiplus::GraphicsPath headPath;
    headPath.AddEllipse(cx - headW * 0.5f, cy - headH * 0.5f, headW, headH);

    Gdiplus::SolidBrush skinBrush(m_skinBaseColor);
    g.FillPath(&skinBrush, &headPath);

    // Shading highlight on cheekbones and forehead
    Gdiplus::SolidBrush highlightBrush(Gdiplus::Color(70, m_skinHighlightColor.GetR(), m_skinHighlightColor.GetG(), m_skinHighlightColor.GetB()));
    g.FillEllipse(&highlightBrush, cx - headW * 0.32f, cy - headH * 0.38f, headW * 0.64f, headH * 0.45f);

    // Subtle jaw contour
    Gdiplus::Pen contourPen(Gdiplus::Color(80, 100, 105, 120), 1.5f * s);
    g.DrawPath(&contourPen, &headPath);
}

void FaceModel::drawCyberneticPlates(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Sleek titanium cranial plates, ear modules, and temple seams
    Gdiplus::SolidBrush plateBrush(m_plateAccentColor);
    Gdiplus::Pen seamPen(Gdiplus::Color(180, 70, 80, 100), 1.2f * s);

    // Temporal ear modules
    float earW = 14.0f * s;
    float earH = 34.0f * s;
    g.FillEllipse(&plateBrush, cx - 64.0f * s - earW, cy - 8.0f * s, earW, earH);
    g.DrawEllipse(&seamPen, cx - 64.0f * s - earW, cy - 8.0f * s, earW, earH);
    
    g.FillEllipse(&plateBrush, cx + 64.0f * s, cy - 8.0f * s, earW, earH);
    g.DrawEllipse(&seamPen, cx + 64.0f * s, cy - 8.0f * s, earW, earH);

    // Glowing ear core
    Gdiplus::SolidBrush earGlow(m_irisGlowColor);
    g.FillEllipse(&earGlow, cx - 64.0f * s - earW * 0.7f, cy + 4.0f * s, 6.0f * s, 10.0f * s);
    g.FillEllipse(&earGlow, cx + 64.0f * s + earW * 0.3f, cy + 4.0f * s, 6.0f * s, 10.0f * s);

    // Forehead parietal plate seam
    g.DrawArc(&seamPen, cx - 45.0f * s, cy - 65.0f * s, 90.0f * s, 40.0f * s, 20.0f, 140.0f);
}

void FaceModel::drawEyes(Gdiplus::Graphics& g, float cx, float cy, float s) {
    float eyeSpacing = 32.0f * s;
    float eyeY = cy - 12.0f * s;
    float eyeW = 28.0f * s;
    float eyeH = 17.0f * s;

    // Draw left and right eye
    drawEye(g, cx - eyeSpacing, eyeY, eyeW, eyeH, false);
    drawEye(g, cx + eyeSpacing, eyeY, eyeW, eyeH, true);
}

void FaceModel::drawEye(Gdiplus::Graphics& g, float ex, float ey, float ew, float eh, bool isRight) {
    float s = ew / 28.0f;
    float openness = clampf(m_currentParams.eyeOpenness, 0.05f, 1.3f);
    float activeH = eh * openness;

    // Sclera socket background
    Gdiplus::GraphicsPath eyePath;
    eyePath.AddEllipse(ex - ew * 0.5f, ey - activeH * 0.5f, ew, activeH);

    Gdiplus::SolidBrush scleraBrush(Gdiplus::Color(255, 245, 248, 252));
    g.FillPath(&scleraBrush, &eyePath);

    // Iris & Gaze positioning
    float maxGazeOffset = 6.0f * s;
    float irisCenterX = ex + m_currentParams.eyeGazeX * maxGazeOffset;
    float irisCenterY = ey + m_currentParams.eyeGazeY * maxGazeOffset * 0.6f;
    float irisRadius = 7.5f * s;

    // Cybernetic luminous Iris
    Gdiplus::SolidBrush irisBrush(m_irisGlowColor);
    g.FillEllipse(&irisBrush, irisCenterX - irisRadius, irisCenterY - irisRadius, irisRadius * 2.0f, irisRadius * 2.0f);

    // Iris outer concentric reticle / detail ring
    Gdiplus::Pen irisRing(Gdiplus::Color(180, 255, 255, 255), 1.0f * s);
    g.DrawEllipse(&irisRing, irisCenterX - irisRadius * 0.8f, irisCenterY - irisRadius * 0.8f, irisRadius * 1.6f, irisRadius * 1.6f);

    // Pupil with dynamic dilation
    float pupilRadius = 3.8f * s * m_currentParams.pupilDilation;
    Gdiplus::SolidBrush pupilBrush(Gdiplus::Color(255, 10, 15, 25));
    g.FillEllipse(&pupilBrush, irisCenterX - pupilRadius, irisCenterY - pupilRadius, pupilRadius * 2.0f, pupilRadius * 2.0f);

    // Specular corneal reflections for lifelike depth
    Gdiplus::SolidBrush glintBrush(Gdiplus::Color(255, 255, 255, 255));
    g.FillEllipse(&glintBrush, irisCenterX - 3.2f * s, irisCenterY - 3.5f * s, 3.2f * s, 3.2f * s);
    g.FillEllipse(&glintBrush, irisCenterX + 1.5f * s, irisCenterY + 1.8f * s, 1.6f * s, 1.6f * s);

    // Upper and lower eyelids / contour
    Gdiplus::Pen lidPen(Gdiplus::Color(200, 60, 65, 80), 1.8f * s);
    g.DrawArc(&lidPen, ex - ew * 0.52f, ey - activeH * 0.52f, ew * 1.04f, activeH * 1.04f, 180.0f, 180.0f);
    g.DrawArc(&lidPen, ex - ew * 0.52f, ey - activeH * 0.52f, ew * 1.04f, activeH * 1.04f, 0.0f, 180.0f);
}

void FaceModel::drawEyebrows(Gdiplus::Graphics& g, float cx, float cy, float s) {
    float browSpacing = 32.0f * s;
    float baseBrowY = cy - 27.0f * s;
    float browW = 26.0f * s;

    Gdiplus::Pen browPen(Gdiplus::Color(220, 65, 50, 45), 2.6f * s);
    browPen.SetStartCap(Gdiplus::LineCapRound);
    browPen.SetEndCap(Gdiplus::LineCapRound);

    // Left Eyebrow (from observer's perspective)
    float lInnerX = cx - 10.0f * s;
    float lInnerY = baseBrowY - m_currentParams.browInnerY * 8.0f * s;
    float lOuterX = cx - browSpacing - browW * 0.5f;
    float lOuterY = baseBrowY - m_currentParams.browOuterY * 6.0f * s + 3.0f * s;
    float lMidX = (lInnerX + lOuterX) * 0.5f;
    float lMidY = baseBrowY - (m_currentParams.browInnerY * 0.5f + m_currentParams.browArch * 5.0f) * s;

    Gdiplus::PointF leftPts[3] = {
        Gdiplus::PointF(lOuterX, lOuterY),
        Gdiplus::PointF(lMidX, lMidY),
        Gdiplus::PointF(lInnerX, lInnerY)
    };
    g.DrawCurve(&browPen, leftPts, 3);

    // Right Eyebrow
    float rInnerX = cx + 10.0f * s;
    float rInnerY = baseBrowY - m_currentParams.browInnerY * 8.0f * s;
    float rOuterX = cx + browSpacing + browW * 0.5f;
    float rOuterY = baseBrowY - m_currentParams.browOuterY * 6.0f * s + 3.0f * s;
    float rMidX = (rInnerX + rOuterX) * 0.5f;
    float rMidY = baseBrowY - (m_currentParams.browInnerY * 0.5f + m_currentParams.browArch * 5.0f) * s;

    Gdiplus::PointF rightPts[3] = {
        Gdiplus::PointF(rInnerX, rInnerY),
        Gdiplus::PointF(rMidX, rMidY),
        Gdiplus::PointF(rOuterX, rOuterY)
    };
    g.DrawCurve(&browPen, rightPts, 3);
}

void FaceModel::drawNose(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Subtle, elegant human nose bridge and tip
    float noseY = cy + 5.0f * s;
    
    // Bridge shadow
    Gdiplus::Pen bridgePen(Gdiplus::Color(45, 140, 120, 110), 1.5f * s);
    g.DrawLine(&bridgePen, cx - 1.5f * s, cy - 8.0f * s, cx - 2.5f * s, noseY + 4.0f * s);

    // Soft tip curve & nostril accents
    Gdiplus::Pen tipPen(Gdiplus::Color(90, 160, 130, 125), 1.8f * s);
    g.DrawArc(&tipPen, cx - 5.5f * s, noseY + 2.0f * s, 11.0f * s, 6.0f * s, 10.0f, 160.0f);
}

void FaceModel::drawMouth(Gdiplus::Graphics& g, float cx, float cy, float s) {
    float mouthY = cy + 30.0f * s;
    float baseW = 28.0f * s * m_currentParams.mouthWidth;
    float openness = m_currentParams.mouthOpenness * 18.0f * s;
    float smileOffset = m_currentParams.mouthSmile * 7.0f * s;

    float leftX = cx - baseW * 0.5f;
    float rightX = cx + baseW * 0.5f;
    float cornerY = mouthY - smileOffset;

    // Oral cavity & teeth when speaking
    if (openness > 1.5f * s) {
        Gdiplus::GraphicsPath oralCavity;
        oralCavity.AddEllipse(cx - baseW * 0.42f, mouthY - openness * 0.4f, baseW * 0.84f, openness * 0.85f);
        
        Gdiplus::SolidBrush cavityBrush(Gdiplus::Color(255, 60, 20, 25));
        g.FillPath(&cavityBrush, &oralCavity);

        // White teeth visible during speech
        Gdiplus::SolidBrush teethBrush(Gdiplus::Color(255, 245, 245, 248));
        g.FillRectangle(&teethBrush, cx - baseW * 0.28f, mouthY - openness * 0.35f, baseW * 0.56f, openness * 0.38f);
    }

    // Upper lip with Cupid's bow
    Gdiplus::GraphicsPath upperLip;
    Gdiplus::PointF upperPts[5] = {
        Gdiplus::PointF(leftX, cornerY),
        Gdiplus::PointF(cx - baseW * 0.2f, mouthY - 3.5f * s - openness * 0.3f),
        Gdiplus::PointF(cx, mouthY - 1.5f * s - openness * 0.3f),
        Gdiplus::PointF(cx + baseW * 0.2f, mouthY - 3.5f * s - openness * 0.3f),
        Gdiplus::PointF(rightX, cornerY)
    };
    upperLip.AddCurve(upperPts, 5);

    Gdiplus::Pen lipPen(m_lipColor, 2.8f * s);
    lipPen.SetStartCap(Gdiplus::LineCapRound);
    lipPen.SetEndCap(Gdiplus::LineCapRound);
    g.DrawPath(&lipPen, &upperLip);

    // Lower lip with soft contour
    Gdiplus::GraphicsPath lowerLip;
    Gdiplus::PointF lowerPts[3] = {
        Gdiplus::PointF(leftX, cornerY),
        Gdiplus::PointF(cx, mouthY + 5.0f * s + openness * 0.6f),
        Gdiplus::PointF(rightX, cornerY)
    };
    lowerLip.AddCurve(lowerPts, 3);
    
    Gdiplus::Pen lowerLipPen(m_lipColor, 3.2f * s);
    lowerLipPen.SetStartCap(Gdiplus::LineCapRound);
    lowerLipPen.SetEndCap(Gdiplus::LineCapRound);
    g.DrawPath(&lowerLipPen, &lowerLip);
}

void FaceModel::drawHUDSensors(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Subtle technological telemetry overlaid near temple/cheek
    Gdiplus::Pen hudPen(Gdiplus::Color(100, m_irisGlowColor.GetR(), m_irisGlowColor.GetG(), m_irisGlowColor.GetB()), 1.0f * s);
    
    // Tech corner brackets
    float bx = cx + 46.0f * s;
    float by = cy - 25.0f * s;
    g.DrawLine(&hudPen, bx, by, bx + 10.0f * s, by);
    g.DrawLine(&hudPen, bx + 10.0f * s, by, bx + 10.0f * s, by + 10.0f * s);

    // Dynamic optical scanner pulse line
    float scanY = cy - 15.0f * s + std::sin(m_timeElapsed * 3.0f) * 12.0f * s;
    Gdiplus::Pen scanPen(Gdiplus::Color(60, m_irisGlowColor.GetR(), m_irisGlowColor.GetG(), m_irisGlowColor.GetB()), 1.0f * s);
    g.DrawLine(&scanPen, cx - 40.0f * s, scanY, cx + 40.0f * s, scanY);
}

} // namespace CosmicRobotics
