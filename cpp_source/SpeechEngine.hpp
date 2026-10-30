#pragma once

#include "Common.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <string>

namespace CosmicRobotics {

struct SpeechTask {
    RobotID speaker;
    std::wstring text;
    Emotion emotion;
    int rate;
    int pitch;
    std::wstring xmlPayload;
};

class SpeechEngine {
public:
    SpeechEngine();
    ~SpeechEngine();

    bool initialize();
    void shutdown();

    void queueSpeech(RobotID speaker, const std::wstring& text, Emotion emotion = EMOTION_SPEAKING, int rate = 0, int pitch = 0);
    void stopCurrentSpeech();
    void clearQueue();

    bool isSpeaking() const { return m_isSpeaking; }
    RobotID getCurrentSpeaker() const { return m_currentSpeaker; }
    std::wstring getCurrentText() const;
    float getSpeechProgress() const;
    Viseme getCurrentViseme() const;

    void update(float dt);

    void playSatelliteSignalChirp();

private:
    void workerThreadFunc();
    void speakTextSync(const std::wstring& text, int rate, int pitch);

    std::atomic<bool> m_running;
    std::atomic<bool> m_isSpeaking;
    std::atomic<RobotID> m_currentSpeaker;
    
    mutable std::mutex m_mutex;
    std::queue<SpeechTask> m_speechQueue;
    std::thread m_workerThread;

    std::wstring m_currentText;
    float m_speechDurationEst;
    float m_speechElapsed;
    Viseme m_currentViseme;

    // COM dispatch pointer for SAPI.SpVoice
    IDispatch* m_pVoice;
    bool m_sapiAvailable;
};

} // namespace CosmicRobotics
