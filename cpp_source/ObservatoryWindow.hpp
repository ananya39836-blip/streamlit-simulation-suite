#pragma once

#include "Common.hpp"
#include "Robot.hpp"
#include "SpeechEngine.hpp"
#include "SatelliteSignals.hpp"
#include "Instruments.hpp"
#include <vector>
#include <memory>

namespace CosmicRobotics {

class ObservatoryWindow {
public:
    ObservatoryWindow();
    ~ObservatoryWindow();

    bool initialize(HINSTANCE hInstance, int width, int height);
    void run();

    void triggerRobotSpeech(RobotID id);
    void triggerSatellitePulse();
    void cycleSpeed();
    void toggleView();

    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    void update(float dt);
    void render(HDC hdc);

    void renderLeftHalf(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderRightHalf(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderDividerAndSignals(Gdiplus::Graphics& g, float x, float y, float h);
    void renderControls(Gdiplus::Graphics& g, float x, float y, float w, float h);
    void renderSubtitleBanner(Gdiplus::Graphics& g, float x, float y, float w, float h);

    void onResize(int width, int height);
    void onMouseDown(int mouseX, int mouseY);

    HWND m_hwnd;
    HINSTANCE m_hInstance;
    int m_width;
    int m_height;

    // GDI+ tokens
    ULONG_PTR m_gdiplusToken;

    // Subsystems
    std::unique_ptr<SpeechEngine> m_speech;
    std::unique_ptr<SatelliteSignals> m_signals;
    std::unique_ptr<Instruments> m_instruments;
    std::vector<std::unique_ptr<Robot>> m_robots;

    // Timing
    std::chrono::high_resolution_clock::time_point m_lastTime;
    float m_appTime;

    // Selection & UI
    RobotID m_activeRobotId;
    std::wstring m_currentSubtitle;
    float m_subtitleFade;
};

} // namespace CosmicRobotics
