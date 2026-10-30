#include "SpeechEngine.hpp"
#include <oleauto.h>
#include <chrono>
#include <sstream>

namespace CosmicRobotics {

SpeechEngine::SpeechEngine()
    : m_running(false)
    , m_isSpeaking(false)
    , m_currentSpeaker(ROBOT_CORNER)
    , m_speechDurationEst(0.0f)
    , m_speechElapsed(0.0f)
    , m_currentViseme(VISEME_REST)
    , m_pVoice(nullptr)
    , m_sapiAvailable(false)
{
}

SpeechEngine::~SpeechEngine() {
    shutdown();
}

bool SpeechEngine::initialize() {
    m_running = true;
    m_workerThread = std::thread(&SpeechEngine::workerThreadFunc, this);
    return true;
}

void SpeechEngine::shutdown() {
    if (m_running) {
        m_running = false;
        clearQueue();
        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
    }
}

void SpeechEngine::queueSpeech(RobotID speaker, const std::wstring& text, Emotion emotion, int rate, int pitch) {
    std::lock_guard<std::mutex> lock(m_mutex);
    SpeechTask task;
    task.speaker = speaker;
    task.text = text;
    task.emotion = emotion;
    task.rate = rate;
    task.pitch = pitch;
    m_speechQueue.push(task);
}

void SpeechEngine::stopCurrentSpeech() {
    // We can clear pending queue
    clearQueue();
}

void SpeechEngine::clearQueue() {
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_speechQueue.empty()) {
        m_speechQueue.pop();
    }
}

std::wstring SpeechEngine::getCurrentText() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentText;
}

float SpeechEngine::getSpeechProgress() const {
    if (m_speechDurationEst <= 0.01f) return 0.0f;
    return clampf(m_speechElapsed / m_speechDurationEst, 0.0f, 1.0f);
}

Viseme SpeechEngine::getCurrentViseme() const {
    return m_currentViseme;
}

void SpeechEngine::update(float dt) {
    if (m_isSpeaking) {
        m_speechElapsed += dt;
        
        // Dynamic procedural viseme generator synchronized with speech cadence
        // Alternates mouth shapes every ~110ms with smooth variations
        float visemeCycle = std::fmod(m_speechElapsed * 8.5f, 5.0f);
        int cycleIndex = static_cast<int>(visemeCycle);
        
        // Add random micro-variations
        if (m_speechElapsed > m_speechDurationEst * 0.95f) {
            m_currentViseme = VISEME_REST;
        } else {
            switch (cycleIndex) {
                case 0: m_currentViseme = VISEME_AA; break;
                case 1: m_currentViseme = VISEME_EE; break;
                case 2: m_currentViseme = VISEME_OH; break;
                case 3: m_currentViseme = VISEME_MM; break;
                case 4: m_currentViseme = VISEME_AA; break;
                default: m_currentViseme = VISEME_REST; break;
            }
        }
    } else {
        m_currentViseme = VISEME_REST;
    }
}

void SpeechEngine::workerThreadFunc() {
    HRESULT hr = CoInitialize(NULL);
    CLSID clsid;
    hr = CLSIDFromProgID(L"SAPI.SpVoice", &clsid);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IDispatch, (void**)&m_pVoice);
        if (SUCCEEDED(hr)) {
            m_sapiAvailable = true;
        }
    }

    while (m_running) {
        SpeechTask currentTask;
        bool hasTask = false;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_speechQueue.empty()) {
                currentTask = m_speechQueue.front();
                m_speechQueue.pop();
                hasTask = true;
            }
        }

        if (hasTask) {
            m_currentSpeaker = currentTask.speaker;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_currentText = currentTask.text;
                // Estimate duration: ~14 characters per second + 0.6s intro/outro buffer
                m_speechDurationEst = (float)currentTask.text.length() / 13.5f + 0.6f;
                m_speechElapsed = 0.0f;
            }

            m_isSpeaking = true;
            speakTextSync(currentTask.text, currentTask.rate, currentTask.pitch);
            m_isSpeaking = false;
            
            // Brief natural pause between speech segments
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    if (m_pVoice) {
        m_pVoice->Release();
        m_pVoice = nullptr;
    }
    CoUninitialize();
}

void SpeechEngine::speakTextSync(const std::wstring& text, int rate, int pitch) {
    if (!m_sapiAvailable || !m_pVoice) {
        // Fallback simulation: sleep for duration
        int ms = static_cast<int>(m_speechDurationEst * 1000.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return;
    }

    // Set Rate property if applicable
    DISPID dispidRate;
    OLECHAR* nameRate = (OLECHAR*)L"Rate";
    if (SUCCEEDED(m_pVoice->GetIDsOfNames(IID_NULL, &nameRate, 1, LOCALE_USER_DEFAULT, &dispidRate))) {
        DISPID dispidNamed = DISPID_PROPERTYPUT;
        DISPPARAMS params;
        VARIANTARG arg;
        VariantInit(&arg);
        arg.vt = VT_I4;
        arg.lVal = rate;
        params.rgvarg = &arg;
        params.rgdispidNamedArgs = &dispidNamed;
        params.cArgs = 1;
        params.cNamedArgs = 1;
        m_pVoice->Invoke(dispidRate, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &params, NULL, NULL, NULL);
    }

    // Call Speak
    DISPID dispidSpeak;
    OLECHAR* nameSpeak = (OLECHAR*)L"Speak";
    if (SUCCEEDED(m_pVoice->GetIDsOfNames(IID_NULL, &nameSpeak, 1, LOCALE_USER_DEFAULT, &dispidSpeak))) {
        DISPPARAMS params;
        VARIANTARG args[2];
        VariantInit(&args[0]); // Flags = 0 (synchronous on this worker thread)
        args[0].vt = VT_I4;
        args[0].lVal = 0; // SVSFDefault (synchronous)

        // Construct optional XML markup for pitch if needed
        std::wstring speechText = text;
        if (pitch != 0) {
            std::wstringstream ss;
            ss << L"<pitch absmiddle=\"" << pitch << L"\">" << text << L"</pitch>";
            speechText = ss.str();
            args[0].lVal = 0; // Synchronous
        }

        VariantInit(&args[1]);
        args[1].vt = VT_BSTR;
        args[1].bstrVal = SysAllocString(speechText.c_str());

        params.rgvarg = args;
        params.rgdispidNamedArgs = NULL;
        params.cArgs = 2;
        params.cNamedArgs = 0;

        m_pVoice->Invoke(dispidSpeak, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);

        SysFreeString(args[1].bstrVal);
    }
}

void SpeechEngine::playSatelliteSignalChirp() {
    std::thread([]() {
        // Procedural futuristic radio satellite telemetry chirp
        Beep(1850, 45);
        Beep(2350, 60);
        Beep(1420, 80);
    }).detach();
}

} // namespace CosmicRobotics
