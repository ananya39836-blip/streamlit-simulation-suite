#pragma once

#include "Common.hpp"

namespace CosmicRobotics {

struct FaceParameters {
    // Eye state
    float eyeOpenness;     // 0.0 (closed) to 1.0 (fully open)
    float eyeGazeX;        // -1.0 (left) to 1.0 (right)
    float eyeGazeY;        // -1.0 (up) to 1.0 (down)
    float pupilDilation;   // 0.6 to 1.4
    
    // Eyebrows
    float browInnerY;      // -1.0 (furrowed) to 1.0 (raised)
    float browOuterY;      // -1.0 to 1.0
    float browArch;        // 0.0 to 1.0
    
    // Mouth & Lips
    float mouthOpenness;   // 0.0 (closed) to 1.0 (open)
    float mouthWidth;      // 0.6 to 1.4
    float mouthSmile;      // -1.0 (frown) to 1.0 (smile)
    float lipSeparation;   // vertical separation of lip surfaces
    
    // Head gestures & orientation
    float headPitch;       // Nodding up/down
    float headYaw;         // Turning left/right
    float headRoll;        // Tilting side-to-side
    float breathingCycle;  // Subtle chest/shoulder breathing
    
    // Cybernetic accents
    float glowIntensity;
    
    FaceParameters()
        : eyeOpenness(1.0f)
        , eyeGazeX(0.0f)
        , eyeGazeY(0.0f)
        , pupilDilation(1.0f)
        , browInnerY(0.0f)
        , browOuterY(0.0f)
        , browArch(0.5f)
        , mouthOpenness(0.0f)
        , mouthWidth(1.0f)
        , mouthSmile(0.18f)
        , lipSeparation(0.0f)
        , headPitch(0.0f)
        , headYaw(0.0f)
        , headRoll(0.0f)
        , breathingCycle(0.0f)
        , glowIntensity(0.85f)
    {}
};

class FaceModel {
public:
    FaceModel(RobotID robotId);
    ~FaceModel();

    void setEmotion(Emotion emotion);
    Emotion getEmotion() const { return m_targetEmotion; }

    void setViseme(Viseme viseme);
    void setGazeTarget(float targetX, float targetY);
    void triggerBlink();

    void update(float dt);
    void render(Gdiplus::Graphics& g, float centerX, float centerY, float scale);

    const FaceParameters& getParams() const { return m_currentParams; }

private:
    void updateBlinking(float dt);
    void updateMicroMovements(float dt);
    void interpolateParameters(float dt);
    FaceParameters getEmotionTargetParams(Emotion emotion) const;
    void applyVisemeToParams(Viseme viseme, FaceParameters& params) const;

    // Rendering routines specifically modeled on the HRP-4C Humanoid Robot (uploaded image)
    void drawChassisAndShoulders(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawHairBack(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawHeadBase(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawEyes(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawEye(Gdiplus::Graphics& g, float ex, float ey, float ew, float eh, bool isRight);
    void drawEyebrows(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawNose(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawMouth(Gdiplus::Graphics& g, float cx, float cy, float s);
    void drawHairFrontBangs(Gdiplus::Graphics& g, float cx, float cy, float s);

    RobotID m_robotId;
    FaceParameters m_currentParams;
    FaceParameters m_targetParams;
    
    Emotion m_targetEmotion;
    Viseme m_currentViseme;

    float m_blinkTimer;
    float m_nextBlinkInterval;
    bool m_isBlinking;
    float m_blinkProgress;

    float m_timeElapsed;
    float m_saccadeTimer;
    Vec2f m_saccadeOffset;
    Vec2f m_gazeTarget;

    // Color attributes for realistic silicone human skin, hair, and metallic chassis
    Gdiplus::Color m_skinBaseColor;
    Gdiplus::Color m_skinHighlightColor;
    Gdiplus::Color m_irisColor;
    Gdiplus::Color m_hairColor;
    Gdiplus::Color m_hairHighlightColor;
    Gdiplus::Color m_lipColor;
    Gdiplus::Color m_chassisSilver;
    Gdiplus::Color m_chassisBlack;
};

} // namespace CosmicRobotics
