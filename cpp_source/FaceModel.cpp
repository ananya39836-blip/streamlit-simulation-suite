#include "FaceModel.hpp"
#include <cmath>

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
    // Modeled faithfully on the HRP-4C Humanoid Robot (user's reference image):
    // Soft realistic silicone skin, jet-black bob hair, dark brown/hazel human eyes,
    // natural rose lips, metallic silver sculpted breastplate, matte black shoulder under-suit
    m_skinBaseColor = Gdiplus::Color(255, 245, 235, 226);       // Warm human silicone skin
    m_skinHighlightColor = Gdiplus::Color(255, 255, 248, 242);  // Cheekbone & forehead highlight
    m_irisColor = Gdiplus::Color(255, 52, 38, 30);              // Deep brown/hazel human iris
    m_hairColor = Gdiplus::Color(255, 22, 20, 24);              // Jet-black hair
    m_hairHighlightColor = Gdiplus::Color(255, 65, 62, 72);     // Subtle sheen highlight on hair
    m_lipColor = Gdiplus::Color(255, 218, 128, 132);            // Natural soft rose lip tone
    m_chassisSilver = Gdiplus::Color(255, 225, 230, 238);       // Metallic sculpted breastplate
    m_chassisBlack = Gdiplus::Color(255, 24, 26, 32);           // Matte black joint under-suit

    // Custom accents for other specialized robots in the squad
    if (m_robotId == ROBOT_UNOBSERVABLE) {
        m_irisColor = Gdiplus::Color(255, 75, 45, 95);          // Cosmic violet iris tint
    } else if (m_robotId == ROBOT_ODOMETER) {
        m_irisColor = Gdiplus::Color(255, 35, 65, 75);          // Sea teal iris tint
    } else if (m_robotId == ROBOT_SPEED) {
        m_irisColor = Gdiplus::Color(255, 75, 50, 25);          // Amber brown iris tint
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
            p.browArch = 0.5f;
            p.mouthSmile = 0.18f; // Gentle relaxed resting smile
            p.eyeOpenness = 1.0f;
            p.pupilDilation = 1.0f;
            p.headPitch = 0.0f;
            p.headRoll = 0.0f;
            break;

        case EMOTION_SPEAKING:
            p.browInnerY = 0.22f; // Expressive eyebrow raise
            p.browOuterY = 0.10f;
            p.browArch = 0.6f;
            p.mouthSmile = 0.35f;
            p.eyeOpenness = 1.05f;
            p.pupilDilation = 1.1f;
            p.headPitch = 0.04f;  // Subtle speech nodding gesture
            break;

        case EMOTION_HAPPY:
            p.browInnerY = 0.28f;
            p.browOuterY = 0.22f;
            p.browArch = 0.68f;
            p.mouthSmile = 0.85f; // Warm human smile
            p.eyeOpenness = 0.94f; // Joyful eye squint
            p.pupilDilation = 1.15f;
            p.headRoll = 0.05f;   // Playful head tilt
            break;

        case EMOTION_THOUGHTFUL:
            p.browInnerY = -0.15f;
            p.browOuterY = 0.25f;
            p.browArch = 0.65f;
            p.mouthSmile = 0.02f;
            p.eyeOpenness = 0.92f;
            p.pupilDilation = 0.95f;
            p.headPitch = -0.05f;
            p.headRoll = 0.08f;   // Contemplative head tilt
            break;

        case EMOTION_SURPRISED:
            p.browInnerY = 0.70f; // High raised eyebrows
            p.browOuterY = 0.65f;
            p.browArch = 0.88f;
            p.mouthSmile = 0.12f;
            p.eyeOpenness = 1.30f; // Wide open eyes
            p.pupilDilation = 1.35f;
            p.headPitch = -0.08f;
            break;

        case EMOTION_ANALYTICAL:
            p.browInnerY = -0.32f; // Focused furrowed brow
            p.browOuterY = -0.08f;
            p.browArch = 0.38f;
            p.mouthSmile = -0.05f;
            p.eyeOpenness = 0.90f;
            p.pupilDilation = 0.88f;
            p.headPitch = 0.06f;
            break;

        case EMOTION_AWE:
            p.browInnerY = 0.48f;
            p.browOuterY = 0.38f;
            p.browArch = 0.78f;
            p.mouthSmile = 0.45f;
            p.eyeOpenness = 1.22f;
            p.pupilDilation = 1.25f;
            p.headPitch = -0.06f;
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
        case VISEME_AA: // "Ah", "Ha" - open natural jaw
            params.mouthOpenness = 0.68f;
            params.mouthWidth = 1.05f;
            params.lipSeparation = 0.55f;
            break;
        case VISEME_EE: // "Ee", "See" - wide stretched lips
            params.mouthOpenness = 0.32f;
            params.mouthWidth = 1.40f;
            params.lipSeparation = 0.28f;
            break;
        case VISEME_OH: // "Oh", "You" - rounded circular lips
            params.mouthOpenness = 0.58f;
            params.mouthWidth = 0.65f;
            params.lipSeparation = 0.42f;
            break;
        case VISEME_MM: // "Mmm", "B", "P" - pressed lips
            params.mouthOpenness = 0.04f;
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
            m_nextBlinkInterval = randFloat(2.5f, 4.8f);
        }
    } else {
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
    
    // Natural human micro-saccades (subtle eye twitches tracking user)
    m_saccadeTimer += dt;
    if (m_saccadeTimer > randFloat(1.2f, 2.6f)) {
        m_saccadeTimer = 0.0f;
        m_saccadeOffset.x = randFloat(-0.035f, 0.035f);
        m_saccadeOffset.y = randFloat(-0.025f, 0.025f);
    }

    // Natural human breathing motion and gentle head gesture sway
    float breath = std::sin(m_timeElapsed * 1.6f) * 0.020f;
    float swayYaw = std::sin(m_timeElapsed * 0.9f) * 0.015f;
    float nodGesture = std::sin(m_timeElapsed * 2.2f) * 0.012f;
    
    m_currentParams.headPitch += breath + nodGesture;
    m_currentParams.headYaw += swayYaw;
    m_currentParams.breathingCycle = breath;
}

void FaceModel::interpolateParameters(float dt) {
    FaceParameters goal = m_targetParams;
    applyVisemeToParams(m_currentViseme, goal);

    goal.eyeGazeX = m_gazeTarget.x + m_saccadeOffset.x;
    goal.eyeGazeY = m_gazeTarget.y + m_saccadeOffset.y;

    float blinkFactor = 1.0f;
    if (m_isBlinking) {
        if (m_blinkProgress < 0.38f) {
            blinkFactor = 1.0f - (m_blinkProgress / 0.38f);
        } else {
            blinkFactor = (m_blinkProgress - 0.38f) / 0.62f;
        }
    }
    goal.eyeOpenness *= blinkFactor;

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
    m_currentParams.headPitch     = lerpf(m_currentParams.headPitch, goal.headPitch, t * 0.8f);
    m_currentParams.headYaw       = lerpf(m_currentParams.headYaw, goal.headYaw, t * 0.8f);
    m_currentParams.headRoll      = lerpf(m_currentParams.headRoll, goal.headRoll, t * 0.8f);
}

void FaceModel::update(float dt) {
    updateBlinking(dt);
    updateMicroMovements(dt);
    interpolateParameters(dt);
}

void FaceModel::render(Gdiplus::Graphics& g, float centerX, float centerY, float scale) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    float hx = centerX + m_currentParams.headYaw * 20.0f * scale;
    float hy = centerY + m_currentParams.headPitch * 15.0f * scale;

    // 1. Lower Cybernetic Torso & Sculpted Silver Breastplate (matching HRP-4C image)
    drawChassisAndShoulders(g, hx, hy, scale);

    // 2. Black Hair (Back Layer falling behind neck and cheeks)
    drawHairBack(g, hx, hy, scale);

    // 3. Human Silicone Face Base
    drawHeadBase(g, hx, hy, scale);

    // 4. Facial Features (Almond human eyes, delicate nose, lips with Cupid's bow)
    drawNose(g, hx, hy, scale);
    drawEyes(g, hx, hy, scale);
    drawEyebrows(g, hx, hy, scale);
    drawMouth(g, hx, hy, scale);

    // 5. Black Hair (Front Bangs/Fringe covering upper forehead exactly as in photo)
    drawHairFrontBangs(g, hx, hy, scale);
}

void FaceModel::drawChassisAndShoulders(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Exact reconstruction of the HRP-4C sculpted metallic silver cuirass and black suit
    float chestY = cy + 62.0f * s;

    // Matte black neck and shoulder under-suit
    Gdiplus::SolidBrush blackSuitBrush(m_chassisBlack);
    g.FillRectangle(&blackSuitBrush, cx - 72.0f * s, chestY - 10.0f * s, 144.0f * s, 85.0f * s);

    // Metallic silver neck collar
    Gdiplus::SolidBrush neckCollar(Gdiplus::Color(255, 140, 148, 160));
    g.FillRectangle(&neckCollar, cx - 22.0f * s, cy + 40.0f * s, 44.0f * s, 26.0f * s);

    Gdiplus::Pen collarSeam(Gdiplus::Color(255, 70, 75, 85), 1.5f * s);
    g.DrawRectangle(&collarSeam, cx - 22.0f * s, cy + 40.0f * s, 44.0f * s, 26.0f * s);

    // Silver Armored Shoulder Pauldrons (Left & Right)
    Gdiplus::SolidBrush silverBrush(m_chassisSilver);
    Gdiplus::Pen seamPen(Gdiplus::Color(255, 110, 120, 135), 1.4f * s);

    // Left shoulder
    g.FillEllipse(&silverBrush, cx - 82.0f * s, chestY - 2.0f * s, 34.0f * s, 48.0f * s);
    g.DrawEllipse(&seamPen, cx - 82.0f * s, chestY - 2.0f * s, 34.0f * s, 48.0f * s);

    // Right shoulder
    g.FillEllipse(&silverBrush, cx + 48.0f * s, chestY - 2.0f * s, 34.0f * s, 48.0f * s);
    g.DrawEllipse(&seamPen, cx + 48.0f * s, chestY - 2.0f * s, 34.0f * s, 48.0f * s);

    // Aerodynamic Contoured Feminine Silver Breastplate (Cuirass)
    Gdiplus::GraphicsPath chestPath;
    Gdiplus::PointF chestPts[7] = {
        Gdiplus::PointF(cx - 45.0f * s, chestY + 6.0f * s),
        Gdiplus::PointF(cx - 30.0f * s, chestY + 2.0f * s),
        Gdiplus::PointF(cx, chestY + 10.0f * s),
        Gdiplus::PointF(cx + 30.0f * s, chestY + 2.0f * s),
        Gdiplus::PointF(cx + 45.0f * s, chestY + 6.0f * s),
        Gdiplus::PointF(cx + 38.0f * s, chestY + 55.0f * s),
        Gdiplus::PointF(cx - 38.0f * s, chestY + 55.0f * s)
    };
    chestPath.AddPolygon(chestPts, 7);
    g.FillPath(&silverBrush, &chestPath);
    g.DrawPath(&seamPen, &chestPath);

    // Breastplate twin sculpted aerodynamic dome contours
    Gdiplus::SolidBrush domeHighlight(Gdiplus::Color(60, 255, 255, 255));
    g.FillEllipse(&domeHighlight, cx - 36.0f * s, chestY + 12.0f * s, 32.0f * s, 30.0f * s);
    g.FillEllipse(&domeHighlight, cx + 4.0f * s, chestY + 12.0f * s, 32.0f * s, 30.0f * s);

    // Upper Clavicle Logo Decal (AIST / Cybernetic Emblem as on photo)
    Gdiplus::SolidBrush decalBg(Gdiplus::Color(255, 20, 24, 30));
    g.FillRectangle(&decalBg, cx + 12.0f * s, chestY + 10.0f * s, 18.0f * s, 8.0f * s);
    
    Gdiplus::Pen decalPen(Gdiplus::Color(255, 0, 200, 255), 0.8f * s);
    g.DrawRectangle(&decalPen, cx + 12.0f * s, chestY + 10.0f * s, 18.0f * s, 8.0f * s);
}

void FaceModel::drawHairBack(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Jet-black bob hairstyle falling straight down along cheeks and behind the neck
    Gdiplus::SolidBrush hairBrush(m_hairColor);
    Gdiplus::Pen hairEdgePen(Gdiplus::Color(255, 12, 10, 14), 1.2f * s);

    // Full back hair silhouette extending down to chin / neck level
    Gdiplus::GraphicsPath hairPath;
    hairPath.AddEllipse(cx - 62.0f * s, cy - 75.0f * s, 124.0f * s, 130.0f * s);
    hairPath.AddRectangle(Gdiplus::RectF(cx - 64.0f * s, cy - 10.0f * s, 128.0f * s, 68.0f * s));
    
    g.FillPath(&hairBrush, &hairPath);
    g.DrawPath(&hairEdgePen, &hairPath);

    // Hair highlights on straight side strands
    Gdiplus::Pen sheenPen(Gdiplus::Color(60, 180, 180, 210), 2.0f * s);
    g.DrawLine(&sheenPen, cx - 55.0f * s, cy - 30.0f * s, cx - 55.0f * s, cy + 45.0f * s);
    g.DrawLine(&sheenPen, cx + 55.0f * s, cy - 30.0f * s, cx + 55.0f * s, cy + 45.0f * s);
}

void FaceModel::drawHeadBase(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Feminine oval silicone face
    float faceW = 88.0f * s;
    float faceH = 112.0f * s;

    Gdiplus::GraphicsPath facePath;
    facePath.AddEllipse(cx - faceW * 0.5f, cy - faceH * 0.5f, faceW, faceH);

    Gdiplus::SolidBrush skinBrush(m_skinBaseColor);
    g.FillPath(&skinBrush, &facePath);

    // Soft blush on cheekbones (subtle peach/rose tint)
    Gdiplus::SolidBrush blushBrush(Gdiplus::Color(35, 235, 120, 130));
    g.FillEllipse(&blushBrush, cx - 38.0f * s, cy + 6.0f * s, 22.0f * s, 16.0f * s);
    g.FillEllipse(&blushBrush, cx + 16.0f * s, cy + 6.0f * s, 22.0f * s, 16.0f * s);

    // Gentle jaw contour
    Gdiplus::Pen jawPen(Gdiplus::Color(45, 160, 130, 120), 1.2f * s);
    g.DrawArc(&jawPen, cx - faceW * 0.48f, cy - faceH * 0.48f, faceW * 0.96f, faceH * 0.96f, 20.0f, 140.0f);
}

void FaceModel::drawEyes(Gdiplus::Graphics& g, float cx, float cy, float s) {
    float eyeSpacing = 24.0f * s;
    float eyeY = cy - 6.0f * s;
    float eyeW = 24.0f * s;
    float eyeH = 15.0f * s;

    drawEye(g, cx - eyeSpacing, eyeY, eyeW, eyeH, false);
    drawEye(g, cx + eyeSpacing, eyeY, eyeW, eyeH, true);
}

void FaceModel::drawEye(Gdiplus::Graphics& g, float ex, float ey, float ew, float eh, bool isRight) {
    float s = ew / 24.0f;
    float openness = clampf(m_currentParams.eyeOpenness, 0.05f, 1.35f);
    float activeH = eh * openness;

    // Almond eye shape
    Gdiplus::GraphicsPath eyePath;
    eyePath.AddEllipse(ex - ew * 0.5f, ey - activeH * 0.5f, ew, activeH);

    Gdiplus::SolidBrush scleraBrush(Gdiplus::Color(255, 250, 252, 255));
    g.FillPath(&scleraBrush, &eyePath);

    // Iris & Gaze positioning
    float maxGazeOffset = 5.0f * s;
    float irisCenterX = ex + m_currentParams.eyeGazeX * maxGazeOffset;
    float irisCenterY = ey + m_currentParams.eyeGazeY * maxGazeOffset * 0.6f;
    float irisRadius = 6.2f * s;

    // Human dark brown/hazel iris (matching HRP-4C robot photo)
    Gdiplus::SolidBrush irisBrush(m_irisColor);
    g.FillEllipse(&irisBrush, irisCenterX - irisRadius, irisCenterY - irisRadius, irisRadius * 2.0f, irisRadius * 2.0f);

    // Outer dark limbal ring
    Gdiplus::Pen limbalRing(Gdiplus::Color(200, 20, 15, 12), 1.0f * s);
    g.DrawEllipse(&limbalRing, irisCenterX - irisRadius, irisCenterY - irisRadius, irisRadius * 2.0f, irisRadius * 2.0f);

    // Black pupil with dynamic emotional dilation
    float pupilRadius = 3.2f * s * m_currentParams.pupilDilation;
    Gdiplus::SolidBrush pupilBrush(Gdiplus::Color(255, 12, 10, 15));
    g.FillEllipse(&pupilBrush, irisCenterX - pupilRadius, irisCenterY - pupilRadius, pupilRadius * 2.0f, pupilRadius * 2.0f);

    // Specular corneal reflection (lively human eyes)
    Gdiplus::SolidBrush glintBrush(Gdiplus::Color(255, 255, 255, 255));
    g.FillEllipse(&glintBrush, irisCenterX - 2.8f * s, irisCenterY - 2.8f * s, 2.8f * s, 2.8f * s);
    g.FillEllipse(&glintBrush, irisCenterX + 1.2f * s, irisCenterY + 1.4f * s, 1.4f * s, 1.4f * s);

    // Upper and lower eyelashes / eyelid line
    Gdiplus::Pen lidPen(Gdiplus::Color(240, 32, 25, 28), 1.8f * s);
    g.DrawArc(&lidPen, ex - ew * 0.52f, ey - activeH * 0.52f, ew * 1.04f, activeH * 1.04f, 180.0f, 180.0f);
    g.DrawArc(&lidPen, ex - ew * 0.52f, ey - activeH * 0.52f, ew * 1.04f, activeH * 1.04f, 0.0f, 180.0f);
}

void FaceModel::drawEyebrows(Gdiplus::Graphics& g, float cx, float cy, float s) {
    float browSpacing = 24.0f * s;
    float baseBrowY = cy - 20.0f * s;
    float browW = 22.0f * s;

    // Delicate dark black/brown human eyebrows
    Gdiplus::Pen browPen(Gdiplus::Color(240, 28, 24, 26), 2.2f * s);
    browPen.SetStartCap(Gdiplus::LineCapRound);
    browPen.SetEndCap(Gdiplus::LineCapRound);

    // Left Brow
    float lInnerX = cx - 8.0f * s;
    float lInnerY = baseBrowY - m_currentParams.browInnerY * 6.0f * s;
    float lOuterX = cx - browSpacing - browW * 0.5f;
    float lOuterY = baseBrowY - m_currentParams.browOuterY * 5.0f * s + 2.5f * s;
    float lMidX = (lInnerX + lOuterX) * 0.5f;
    float lMidY = baseBrowY - (m_currentParams.browInnerY * 0.4f + m_currentParams.browArch * 4.5f) * s;

    Gdiplus::PointF leftPts[3] = {
        Gdiplus::PointF(lOuterX, lOuterY),
        Gdiplus::PointF(lMidX, lMidY),
        Gdiplus::PointF(lInnerX, lInnerY)
    };
    g.DrawCurve(&browPen, leftPts, 3);

    // Right Brow
    float rInnerX = cx + 8.0f * s;
    float rInnerY = baseBrowY - m_currentParams.browInnerY * 6.0f * s;
    float rOuterX = cx + browSpacing + browW * 0.5f;
    float rOuterY = baseBrowY - m_currentParams.browOuterY * 5.0f * s + 2.5f * s;
    float rMidX = (rInnerX + rOuterX) * 0.5f;
    float rMidY = baseBrowY - (m_currentParams.browInnerY * 0.4f + m_currentParams.browArch * 4.5f) * s;

    Gdiplus::PointF rightPts[3] = {
        Gdiplus::PointF(rInnerX, rInnerY),
        Gdiplus::PointF(rMidX, rMidY),
        Gdiplus::PointF(rOuterX, rOuterY)
    };
    g.DrawCurve(&browPen, rightPts, 3);
}

void FaceModel::drawNose(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Feminine refined nose
    float noseY = cy + 10.0f * s;

    Gdiplus::Pen bridgePen(Gdiplus::Color(50, 160, 130, 120), 1.2f * s);
    g.DrawLine(&bridgePen, cx - 1.2f * s, cy - 2.0f * s, cx - 1.8f * s, noseY + 2.0f * s);

    // Tip curve & soft nostril arcs
    Gdiplus::Pen tipPen(Gdiplus::Color(110, 180, 140, 130), 1.5f * s);
    g.DrawArc(&tipPen, cx - 4.5f * s, noseY + 1.0f * s, 9.0f * s, 4.5f * s, 10.0f, 160.0f);
}

void FaceModel::drawMouth(Gdiplus::Graphics& g, float cx, float cy, float s) {
    float mouthY = cy + 30.0f * s;
    float baseW = 24.0f * s * m_currentParams.mouthWidth;
    float openness = m_currentParams.mouthOpenness * 16.0f * s;
    float smileOffset = m_currentParams.mouthSmile * 6.0f * s;

    float leftX = cx - baseW * 0.5f;
    float rightX = cx + baseW * 0.5f;
    float cornerY = mouthY - smileOffset;

    // Oral cavity & teeth when speaking
    if (openness > 1.2f * s) {
        Gdiplus::GraphicsPath oralCavity;
        oralCavity.AddEllipse(cx - baseW * 0.40f, mouthY - openness * 0.4f, baseW * 0.80f, openness * 0.82f);
        
        Gdiplus::SolidBrush cavityBrush(Gdiplus::Color(255, 65, 20, 28));
        g.FillPath(&cavityBrush, &oralCavity);

        // White teeth visible during speech
        Gdiplus::SolidBrush teethBrush(Gdiplus::Color(255, 250, 250, 252));
        g.FillRectangle(&teethBrush, cx - baseW * 0.25f, mouthY - openness * 0.35f, baseW * 0.50f, openness * 0.35f);
    }

    // Upper lip with Cupid's bow
    Gdiplus::GraphicsPath upperLip;
    Gdiplus::PointF upperPts[5] = {
        Gdiplus::PointF(leftX, cornerY),
        Gdiplus::PointF(cx - baseW * 0.18f, mouthY - 3.0f * s - openness * 0.28f),
        Gdiplus::PointF(cx, mouthY - 1.2f * s - openness * 0.28f),
        Gdiplus::PointF(cx + baseW * 0.18f, mouthY - 3.0f * s - openness * 0.28f),
        Gdiplus::PointF(rightX, cornerY)
    };
    upperLip.AddCurve(upperPts, 5);

    Gdiplus::Pen lipPen(m_lipColor, 2.5f * s);
    lipPen.SetStartCap(Gdiplus::LineCapRound);
    lipPen.SetEndCap(Gdiplus::LineCapRound);
    g.DrawPath(&lipPen, &upperLip);

    // Lower lip with soft contour
    Gdiplus::GraphicsPath lowerLip;
    Gdiplus::PointF lowerPts[3] = {
        Gdiplus::PointF(leftX, cornerY),
        Gdiplus::PointF(cx, mouthY + 4.5f * s + openness * 0.55f),
        Gdiplus::PointF(rightX, cornerY)
    };
    lowerLip.AddCurve(lowerPts, 3);
    
    Gdiplus::Pen lowerLipPen(m_lipColor, 2.8f * s);
    lowerLipPen.SetStartCap(Gdiplus::LineCapRound);
    lowerLipPen.SetEndCap(Gdiplus::LineCapRound);
    g.DrawPath(&lowerLipPen, &lowerLip);
}

void FaceModel::drawHairFrontBangs(Gdiplus::Graphics& g, float cx, float cy, float s) {
    // Signature straight horizontal front bangs (fringe) of HRP-4C
    // Neatly cut across the forehead right above the eyebrows
    Gdiplus::SolidBrush hairBrush(m_hairColor);
    Gdiplus::Pen edgePen(Gdiplus::Color(255, 12, 10, 14), 1.0f * s);

    // Front bangs block
    float bangW = 66.0f * s;
    float bangTopY = cy - 65.0f * s;
    float bangBottomY = cy - 25.0f * s; // Cut horizontally right above eyebrows

    Gdiplus::GraphicsPath bangsPath;
    bangsPath.AddRectangle(Gdiplus::RectF(cx - bangW * 0.5f, bangTopY, bangW, bangBottomY - bangTopY));
    
    // Add side strands curving inward
    Gdiplus::PointF leftStrand[4] = {
        Gdiplus::PointF(cx - bangW * 0.5f, bangTopY),
        Gdiplus::PointF(cx - 52.0f * s, cy),
        Gdiplus::PointF(cx - 48.0f * s, cy + 35.0f * s),
        Gdiplus::PointF(cx - 42.0f * s, cy + 42.0f * s)
    };
    bangsPath.AddCurve(leftStrand, 4);

    Gdiplus::PointF rightStrand[4] = {
        Gdiplus::PointF(cx + bangW * 0.5f, bangTopY),
        Gdiplus::PointF(cx + 52.0f * s, cy),
        Gdiplus::PointF(cx + 48.0f * s, cy + 35.0f * s),
        Gdiplus::PointF(cx + 42.0f * s, cy + 42.0f * s)
    };
    bangsPath.AddCurve(rightStrand, 4);

    g.FillPath(&hairBrush, &bangsPath);

    // Bang fringe edge individual strand feathering
    Gdiplus::Pen strandPen(m_hairColor, 1.5f * s);
    for (float bx = cx - bangW * 0.48f; bx <= cx + bangW * 0.48f; bx += 3.5f * s) {
        float strandLen = randFloat(2.0f, 5.0f) * s;
        g.DrawLine(&strandPen, bx, bangBottomY, bx, bangBottomY + strandLen);
    }

    // Horizontal gloss highlight on bangs
    Gdiplus::Pen glossPen(Gdiplus::Color(50, 200, 205, 230), 2.2f * s);
    g.DrawArc(&glossPen, cx - bangW * 0.40f, bangTopY + 12.0f * s, bangW * 0.80f, 15.0f * s, 10.0f, 160.0f);
}

} // namespace CosmicRobotics
