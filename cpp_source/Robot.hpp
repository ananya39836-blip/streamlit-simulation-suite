#pragma once

#include "Common.hpp"
#include "FaceModel.hpp"
#include <string>
#include <memory>
#include <vector>

namespace CosmicRobotics {

class Robot {
public:
    Robot(RobotID id, const std::wstring& name, const std::wstring& title, const std::wstring& roleDescription);
    virtual ~Robot();

    RobotID getId() const { return m_id; }
    const std::wstring& getName() const { return m_name; }
    const std::wstring& getTitle() const { return m_title; }
    const std::wstring& getRoleDescription() const { return m_roleDescription; }

    FaceModel& getFace() { return *m_face; }
    const FaceModel& getFace() const { return *m_face; }

    void setSpeaking(bool speaking, Viseme viseme);
    bool isSpeaking() const { return m_isSpeaking; }

    void setEmotion(Emotion emotion);
    Emotion getEmotion() const { return m_face->getEmotion(); }

    void setGaze(float gx, float gy);

    virtual void update(float dt);
    virtual void renderCard(Gdiplus::Graphics& g, float x, float y, float w, float h, bool isHighlighted);

    // Dialogue prompts
    virtual std::wstring getRandomDialogue() = 0;
    virtual std::wstring getIntroSpeech() = 0;
    virtual std::wstring getCosmicOceanReport() = 0;

protected:
    RobotID m_id;
    std::wstring m_name;
    std::wstring m_title;
    std::wstring m_roleDescription;
    std::unique_ptr<FaceModel> m_face;
    
    bool m_isSpeaking;
    float m_speechAnimTimer;
    float m_statusBlink;
    Gdiplus::Color m_themeColor;
};

// 1. Frontline Corner Robot: Liaison speaking to user on laptop
class RobotCorner : public Robot {
public:
    RobotCorner();
    std::wstring getRandomDialogue() override;
    std::wstring getIntroSpeech() override;
    std::wstring getCosmicOceanReport() override;
    void renderLaptopWorkstation(Gdiplus::Graphics& g, float x, float y, float w, float h);
};

// 2. Deep Space Void Robot: Relaying EVERY DATA collected from unobservable universe
class RobotUnobservable : public Robot {
public:
    RobotUnobservable();
    std::wstring getRandomDialogue() override;
    std::wstring getIntroSpeech() override;
    std::wstring getCosmicOceanReport() override;
};

// 3. Xenobiologist / Cosmic Fish Guardian Robot: Speaking about alien presence & kinds of fish in cosmic sea
class RobotOdometer : public Robot {
public:
    RobotOdometer();
    std::wstring getRandomDialogue() override;
    std::wstring getIntroSpeech() override;
    std::wstring getCosmicOceanReport() override;
};

// 4. Kinematics, Odometer & Relativistic Speed Robot
class RobotSpeed : public Robot {
public:
    RobotSpeed();
    std::wstring getRandomDialogue() override;
    std::wstring getIntroSpeech() override;
    std::wstring getCosmicOceanReport() override;
};

} // namespace CosmicRobotics
