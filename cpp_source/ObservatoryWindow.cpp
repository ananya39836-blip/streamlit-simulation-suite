#include "ObservatoryWindow.hpp"
#include <windowsx.h>

namespace CosmicRobotics {

static ObservatoryWindow* g_pWindow = nullptr;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_pWindow) {
        return g_pWindow->handleMessage(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

ObservatoryWindow::ObservatoryWindow()
    : m_hwnd(NULL)
    , m_hInstance(NULL)
    , m_width(1480)
    , m_height(900)
    , m_gdiplusToken(0)
    , m_appTime(0.0f)
    , m_activeRobotId(ROBOT_CORNER)
    , m_currentSubtitle(L"Cosmic Robotics Observatory initialized. Ready for user interaction.")
    , m_subtitleFade(1.0f)
{
    g_pWindow = this;
}

ObservatoryWindow::~ObservatoryWindow() {
    if (m_speech) {
        m_speech->shutdown();
    }
    if (m_gdiplusToken) {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }
}

bool ObservatoryWindow::initialize(HINSTANCE hInstance, int width, int height) {
    m_hInstance = hInstance;
    m_width = width;
    m_height = height;

    // 1. Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    // 2. Initialize Subsystems
    m_speech = std::unique_ptr<SpeechEngine>(new SpeechEngine());
    m_speech->initialize();

    m_signals = std::unique_ptr<SatelliteSignals>(new SatelliteSignals());
    m_instruments = std::unique_ptr<Instruments>(new Instruments());
    m_instruments->initialize();

    // Initialize 4 specialized robots
    m_robots.push_back(std::unique_ptr<Robot>(new RobotCorner()));
    m_robots.push_back(std::unique_ptr<Robot>(new RobotUnobservable()));
    m_robots.push_back(std::unique_ptr<Robot>(new RobotOdometer()));
    m_robots.push_back(std::unique_ptr<Robot>(new RobotSpeed()));

    // Configure satellite signal trajectory across the cosmos
    // Originates from Chronos-Void in unobservable universe (right half) and streams to EVA / Laptop (left half)
    float halfW = static_cast<float>(m_width) * 0.5f;
    Vec2f unobservableOrigin(halfW + halfW * 0.45f, m_height * 0.22f);
    Vec2f laptopStationTarget(halfW * 0.38f, m_height * 0.48f);
    m_signals->initialize(unobservableOrigin, laptopStationTarget);

    // 3. Register Win32 Window Class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"CosmicObservatoryWindowClass";

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    // 4. Create Window
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    RECT wr = { 0, 0, m_width, m_height };
    AdjustWindowRect(&wr, style, FALSE);

    m_hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"COSMIC ROBOTICS OBSERVATORY :: UNOBSERVABLE UNIVERSE SIMULATOR & RELATIVISTIC TELEMETRY",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL, NULL, hInstance, NULL
    );

    if (!m_hwnd) {
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    m_lastTime = std::chrono::high_resolution_clock::now();

    // Start with EVA's human greeting to the user at laptop
    triggerRobotSpeech(ROBOT_CORNER);

    return true;
}

void ObservatoryWindow::triggerRobotSpeech(RobotID id) {
    if (id < 0 || id >= ROBOT_COUNT) return;
    m_activeRobotId = id;
    
    std::wstring speechText;
    Emotion em = EMOTION_SPEAKING;
    int rate = 0;
    int pitch = 0;

    switch (id) {
        case ROBOT_CORNER:
            speechText = m_robots[0]->getIntroSpeech();
            em = EMOTION_HAPPY;
            rate = 1;
            pitch = 2;
            break;
        case ROBOT_UNOBSERVABLE:
            speechText = m_robots[1]->getIntroSpeech();
            em = EMOTION_ANALYTICAL;
            rate = 0;
            pitch = -3;
            m_signals->triggerBurst();
            break;
        case ROBOT_ODOMETER:
            speechText = m_robots[2]->getIntroSpeech();
            em = EMOTION_AWE;
            rate = 0;
            pitch = 1;
            break;
        case ROBOT_SPEED:
            speechText = m_robots[3]->getIntroSpeech();
            em = EMOTION_SURPRISED;
            rate = 2;
            pitch = 0;
            break;
    }

    m_currentSubtitle = speechText;
    m_subtitleFade = 1.0f;
    m_robots[id]->setEmotion(em);
    m_speech->queueSpeech(id, speechText, em, rate, pitch);
}

void ObservatoryWindow::triggerSatellitePulse() {
    m_signals->triggerBurst();
    m_speech->playSatelliteSignalChirp();
    m_currentSubtitle = L"[SATELLITE TELEMETRY BURST] Quantum carrier frequency 248.84 GHz pulsed from Chronos-Void.";
    m_subtitleFade = 1.0f;
}

void ObservatoryWindow::cycleSpeed() {
    m_instruments->cycleSpeedPreset();
    wchar_t buf[128];
    swprintf_s(buf, L"[VELOCITY CALIBRATION] Tachometer target set to %.3f c (Gamma \u03b3: %.2f)", 
               m_instruments->getSpeedFractionOfC(), m_instruments->getLorentzGamma());
    m_currentSubtitle = buf;
    m_subtitleFade = 1.0f;
}

void ObservatoryWindow::toggleView() {
    m_instruments->toggleViewMode();
    if (m_instruments->getViewMode() == VIEW_COSMIC_OCEAN) {
        m_currentSubtitle = L"[VIEW SWITCHED] Sea of Unobservable Universe: Cosmic Fish & Elder Alien Observer Active.";
    } else {
        m_currentSubtitle = L"[VIEW SWITCHED] Multi-scale 93 Gly Cosmic Horizon & Planetary-Galactic Orbits Active.";
    }
    m_subtitleFade = 1.0f;
}

void ObservatoryWindow::update(float dt) {
    m_appTime += dt;
    m_speech->update(dt);

    bool speaking = m_speech->isSpeaking();
    RobotID currentSpeaker = m_speech->getCurrentSpeaker();
    Viseme curViseme = m_speech->getCurrentViseme();

    for (size_t i = 0; i < m_robots.size(); ++i) {
        bool isThisSpeaking = speaking && (currentSpeaker == static_cast<RobotID>(i));
        m_robots[i]->setSpeaking(isThisSpeaking, isThisSpeaking ? curViseme : VISEME_REST);
        m_robots[i]->update(dt);
    }

    m_signals->update(dt);
    m_instruments->update(dt);

    if (speaking) {
        std::wstring curText = m_speech->getCurrentText();
        if (!curText.empty()) {
            m_currentSubtitle = curText;
        }
    }
}

void ObservatoryWindow::render(HDC hdc) {
    // High-performance double-buffering
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, m_width, m_height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    Gdiplus::Graphics g(memDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // Clear background
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 6, 8, 14));
    g.FillRectangle(&bgBrush, 0, 0, m_width, m_height);

    float halfW = static_cast<float>(m_width) * 0.5f;
    float fullH = static_cast<float>(m_height);

    // 1. LEFT HALF: Robots (Speaking to user at laptop, realistic faces, visemes, controls)
    renderLeftHalf(g, 0.0f, 0.0f, halfW, fullH);

    // 2. RIGHT HALF: Other things (Unobservable ocean, alien presence, orbits, odometer, speedometer, lidar)
    renderRightHalf(g, halfW, 0.0f, halfW, fullH);

    // 3. CENTER DIVIDER & Satellite Signals Emerging from Unobservable Universe
    renderDividerAndSignals(g, halfW, 0.0f, fullH);

    // 4. Bottom Subtitle & Speech Transcription Ticker across the screen
    renderSubtitleBanner(g, 10.0f, fullH - 38.0f, m_width - 20.0f, 30.0f);

    // Blit to screen
    BitBlt(hdc, 0, 0, m_width, m_height, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

void ObservatoryWindow::renderLeftHalf(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    // Header for Robots side
    Gdiplus::SolidBrush headerBg(Gdiplus::Color(255, 12, 18, 30));
    g.FillRectangle(&headerBg, x + 8.0f, y + 8.0f, w - 16.0f, 30.0f);

    Gdiplus::Pen headerBorder(Gdiplus::Color(255, 0, 200, 255), 1.2f);
    g.DrawRectangle(&headerBorder, x + 8.0f, y + 8.0f, w - 16.0f, 30.0f);

    Gdiplus::Font headerFont(L"Consolas", 9.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 0, 240, 255));
    g.DrawString(L"PARTITION A :: CYBERNETIC ROBOTS SQUAD (HUMAN-FACIAL INTERACTION)", -1, &headerFont, Gdiplus::PointF(x + 16.0f, y + 14.0f), &titleBrush);

    // Main Stage Top: Corner Robot EVA-01 with Laptop Workstation
    // "At one corner, these robots will be speaking to the user me who is on laptop"
    float stageY = y + 44.0f;
    float stageH = h * 0.46f;
    float stageW = w - 16.0f;

    Gdiplus::SolidBrush stageBg(Gdiplus::Color(255, 14, 18, 28));
    g.FillRectangle(&stageBg, x + 8.0f, stageY, stageW, stageH);
    
    Gdiplus::Pen stagePen(Gdiplus::Color(255, 0, 220, 255), (m_activeRobotId == ROBOT_CORNER) ? 2.0f : 1.0f);
    g.DrawRectangle(&stagePen, x + 8.0f, stageY, stageW, stageH);

    // Render Corner Robot Face
    float evaFaceX = x + 8.0f + stageW * 0.30f;
    float evaFaceY = stageY + stageH * 0.44f;
    m_robots[0]->getFace().render(g, evaFaceX, evaFaceY, 1.25f);

    // Render User Laptop Workstation on the right side of the stage
    RobotCorner* pCorner = dynamic_cast<RobotCorner*>(m_robots[0].get());
    if (pCorner) {
        float lapX = x + 8.0f + stageW * 0.58f;
        float lapY = stageY + 15.0f;
        float lapW = stageW * 0.38f;
        float lapH = stageH - 30.0f;
        pCorner->renderLaptopWorkstation(g, lapX, lapY, lapW, lapH);
    }

    // EVA Stage Badge
    Gdiplus::Font evaBadgeFont(L"Consolas", 8.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush evaBadgeBrush(Gdiplus::Color(255, 0, 240, 255));
    g.DrawString(L"EVA-01 :: CORNER ROBOT SPEAKING TO USER AT LAPTOP", -1, &evaBadgeFont, Gdiplus::PointF(x + 18.0f, stageY + 10.0f), &evaBadgeBrush);

    // Bottom Grid: 3 Specialized Robot Cards (Chronos-Void, Thalassa-Bio, Velox-Tachyon)
    float gridY = stageY + stageH + 10.0f;
    float gridH = h * 0.32f;
    float cardW = (stageW - 16.0f) / 3.0f;

    for (size_t i = 1; i < m_robots.size(); ++i) {
        float cardX = x + 8.0f + (i - 1) * (cardW + 8.0f);
        bool isAct = (m_activeRobotId == static_cast<RobotID>(i));
        m_robots[i]->renderCard(g, cardX, gridY, cardW, gridH, isAct);
    }

    // Interactive Controls Bar at Bottom of Left Half
    float ctrlY = gridY + gridH + 8.0f;
    float ctrlH = h - ctrlY - 48.0f;
    renderControls(g, x + 8.0f, ctrlY, stageW, ctrlH);
}

void ObservatoryWindow::renderRightHalf(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    // The Right Half contains all the cosmic simulation and scientific instruments:
    // Orbits, Sea of unobservable universe, fish, alien presence, odometer, speedometer, lidar
    m_instruments->render(g, x + 8.0f, y + 8.0f, w - 16.0f, h - 54.0f);
}

void ObservatoryWindow::renderDividerAndSignals(Gdiplus::Graphics& g, float x, float y, float h) {
    // Vertical sci-fi division bar separating Left Half and Right Half
    Gdiplus::Pen divPen(Gdiplus::Color(255, 30, 70, 110), 2.0f);
    g.DrawLine(&divPen, x, y + 8.0f, x, y + h - 45.0f);

    Gdiplus::Pen divGlow(Gdiplus::Color(70, 0, 240, 255), 6.0f);
    g.DrawLine(&divGlow, x, y + 8.0f, x, y + h - 45.0f);

    // Center divider badge
    Gdiplus::SolidBrush badgeBg(Gdiplus::Color(255, 14, 20, 32));
    g.FillRectangle(&badgeBg, x - 55.0f, y + h * 0.48f - 12.0f, 110.0f, 24.0f);

    Gdiplus::Pen badgeBorder(Gdiplus::Color(255, 0, 220, 255), 1.2f);
    g.DrawRectangle(&badgeBorder, x - 55.0f, y + h * 0.48f - 12.0f, 110.0f, 24.0f);

    Gdiplus::Font divFont(L"Consolas", 6.8f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush divBrush(Gdiplus::Color(255, 0, 240, 255));
    g.DrawString(L"HORIZON DIVIDE", -1, &divFont, Gdiplus::PointF(x - 50.0f, y + h * 0.48f - 6.0f), &divBrush);

    // Render the Satellite Signals streaming from unobservable universe across to user
    m_signals->render(g, 0, 0, m_width, m_height);
}

void ObservatoryWindow::renderControls(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    Gdiplus::SolidBrush ctrlBg(Gdiplus::Color(255, 12, 16, 26));
    g.FillRectangle(&ctrlBg, x, y, w, h);

    Gdiplus::Pen ctrlBorder(Gdiplus::Color(255, 40, 65, 90), 1.0f);
    g.DrawRectangle(&ctrlBorder, x, y, w, h);

    Gdiplus::Font btnFont(L"Consolas", 7.2f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);

    // Helper to draw futuristic interactive action buttons
    auto drawBtn = [&](float bx, float by, float bw, float bh, const wchar_t* keyText, const wchar_t* labelText, Gdiplus::Color col) {
        Gdiplus::SolidBrush btnBg(Gdiplus::Color(255, 20, 28, 42));
        g.FillRectangle(&btnBg, bx, by, bw, bh);

        Gdiplus::Pen bPen(col, 1.2f);
        g.DrawRectangle(&bPen, bx, by, bw, bh);

        Gdiplus::SolidBrush keyBrush(col);
        g.DrawString(keyText, -1, &btnFont, Gdiplus::PointF(bx + 4.0f, by + 4.0f), &keyBrush);

        Gdiplus::SolidBrush labelBrush(Gdiplus::Color(255, 220, 230, 245));
        g.DrawString(labelText, -1, &btnFont, Gdiplus::PointF(bx + 4.0f, by + 18.0f), &labelBrush);
    };

    float btnW = (w - 30.0f) / 6.0f;
    float btnH = h - 10.0f;
    float by = y + 5.0f;

    drawBtn(x + 5.0f + 0 * (btnW + 4.0f), by, btnW, btnH, L"[F1]", L"EVA / Laptop", Gdiplus::Color(255, 0, 240, 255));
    drawBtn(x + 5.0f + 1 * (btnW + 4.0f), by, btnW, btnH, L"[F2]", L"Chronos Data", Gdiplus::Color(255, 190, 80, 255));
    drawBtn(x + 5.0f + 2 * (btnW + 4.0f), by, btnW, btnH, L"[F3]", L"Thalassa Fish", Gdiplus::Color(255, 30, 210, 180));
    drawBtn(x + 5.0f + 3 * (btnW + 4.0f), by, btnW, btnH, L"[F4]", L"Velox Speed", Gdiplus::Color(255, 255, 140, 20));
    drawBtn(x + 5.0f + 4 * (btnW + 4.0f), by, btnW, btnH, L"[SPACE]", L"Satellite Pulse", Gdiplus::Color(255, 140, 100, 255));
    drawBtn(x + 5.0f + 5 * (btnW + 4.0f), by, btnW, btnH, L"[TAB]", L"Toggle Sea/Map", Gdiplus::Color(255, 0, 255, 160));
}

void ObservatoryWindow::renderSubtitleBanner(Gdiplus::Graphics& g, float x, float y, float w, float h) {
    Gdiplus::SolidBrush bannerBg(Gdiplus::Color(255, 10, 14, 22));
    g.FillRectangle(&bannerBg, x, y, w, h);

    Gdiplus::Pen bannerPen(Gdiplus::Color(255, 0, 180, 240), 1.2f);
    g.DrawRectangle(&bannerPen, x, y, w, h);

    // Audio status icon
    Gdiplus::SolidBrush iconBrush(m_speech->isSpeaking() ? Gdiplus::Color(255, 0, 255, 180) : Gdiplus::Color(255, 80, 100, 120));
    g.FillEllipse(&iconBrush, x + 10.0f, y + 8.0f, 14.0f, 14.0f);

    Gdiplus::Font subFont(L"Consolas", 8.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 220, 240, 255));

    // Display speaker prefix
    std::wstring dispText = L"[" + m_robots[m_activeRobotId]->getName() + L"]: " + m_currentSubtitle;
    g.DrawString(dispText.c_str(), -1, &subFont, Gdiplus::PointF(x + 32.0f, y + 8.0f), &textBrush);
}

void ObservatoryWindow::onResize(int width, int height) {
    m_width = width;
    m_height = height;
    float halfW = static_cast<float>(m_width) * 0.5f;
    Vec2f unobservableOrigin(halfW + halfW * 0.45f, m_height * 0.22f);
    Vec2f laptopStationTarget(halfW * 0.38f, m_height * 0.48f);
    m_signals->initialize(unobservableOrigin, laptopStationTarget);
}

void ObservatoryWindow::onMouseDown(int mouseX, int mouseY) {
    float halfW = static_cast<float>(m_width) * 0.5f;
    if (mouseX < halfW) {
        // Click on left half
        float stageY = 44.0f;
        float stageH = m_height * 0.46f;
        if (mouseY >= stageY && mouseY <= stageY + stageH) {
            triggerRobotSpeech(ROBOT_CORNER);
            return;
        }

        float gridY = stageY + stageH + 10.0f;
        float gridH = m_height * 0.32f;
        if (mouseY >= gridY && mouseY <= gridY + gridH) {
            float cardW = (halfW - 32.0f) / 3.0f;
            int idx = static_cast<int>((mouseX - 8.0f) / (cardW + 8.0f)) + 1;
            if (idx >= 1 && idx <= 3) {
                triggerRobotSpeech(static_cast<RobotID>(idx));
                return;
            }
        }
    } else {
        // Click on right half toggles simulation view
        toggleView();
    }
}

LRESULT ObservatoryWindow::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE: {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            if (w > 100 && h > 100) {
                onResize(w, h);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);
            onMouseDown(mx, my);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_F1: triggerRobotSpeech(ROBOT_CORNER); break;
                case VK_F2: triggerRobotSpeech(ROBOT_UNOBSERVABLE); break;
                case VK_F3: triggerRobotSpeech(ROBOT_ODOMETER); break;
                case VK_F4: triggerRobotSpeech(ROBOT_SPEED); break;
                case VK_SPACE: triggerSatellitePulse(); break;
                case VK_TAB: toggleView(); break;
                case 'C': cycleSpeed(); break;
                case VK_ESCAPE: PostQuitMessage(0); break;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            render(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1; // Prevent flicker

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ObservatoryWindow::run() {
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            auto now = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float>(now - m_lastTime).count();
            m_lastTime = now;

            if (dt > 0.1f) dt = 0.1f; // Clamp delta time during drags/pauses

            update(dt);
            InvalidateRect(m_hwnd, NULL, FALSE);

            // Cap at ~60 FPS
            Sleep(16);
        }
    }
}

} // namespace CosmicRobotics
