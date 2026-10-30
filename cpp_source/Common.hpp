#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <ole2.h>
#include <oleauto.h>
#include <gdiplus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

#ifndef sprintf_s
#define sprintf_s(buf, fmt, ...) snprintf(buf, sizeof(buf), fmt, ##__VA_ARGS__)
#endif

#ifndef swprintf_s
#define swprintf_s(buf, fmt, ...) _snwprintf(buf, sizeof(buf)/sizeof(wchar_t), fmt, ##__VA_ARGS__)
#endif

namespace CosmicRobotics {

const float PI = 3.14159265358979323846f;
const float TWO_PI = 6.28318530717958647692f;
const double SPEED_OF_LIGHT = 299792.458; // km/s
const double PARSEC_TO_LY = 3.26156;
const double LY_TO_KM = 9.4607e12; // km
const double OBSERVABLE_HORIZON_LY = 46.5e9; // 46.5 billion light-years

enum RobotID {
    ROBOT_CORNER = 0,       // "EVA-01" / Corner Liaison Robot talking to user at laptop
    ROBOT_UNOBSERVABLE = 1, // "CHRONOS-VOID" / Probe in unobservable universe emitting satellite signals
    ROBOT_ODOMETER = 2,     // "THALASSA-BIO" / Guardian of cosmic sea, fish & alien presence
    ROBOT_SPEED = 3,        // "VELOX-TACHYON" / Relativistic velocity & odometer
    ROBOT_COUNT = 4
};

enum Emotion {
    EMOTION_NEUTRAL = 0,
    EMOTION_SPEAKING,
    EMOTION_HAPPY,
    EMOTION_THOUGHTFUL,
    EMOTION_SURPRISED,
    EMOTION_ANALYTICAL,
    EMOTION_AWE
};

enum Viseme {
    VISEME_REST = 0, // Closed / resting mouth
    VISEME_AA,       // Open mouth (A, H)
    VISEME_EE,       // Wide mouth (E, I, Y)
    VISEME_OH,       // Round mouth (O, U, W)
    VISEME_MM        // Pursed lips (M, B, P)
};

struct Vec2f {
    float x;
    float y;
    Vec2f() : x(0.0f), y(0.0f) {}
    Vec2f(float _x, float _y) : x(_x), y(_y) {}
    
    Vec2f operator+(const Vec2f& o) const { return Vec2f(x + o.x, y + o.y); }
    Vec2f operator-(const Vec2f& o) const { return Vec2f(x - o.x, y - o.y); }
    Vec2f operator*(float s) const { return Vec2f(x * s, y * s); }
    float length() const { return std::sqrt(x * x + y * y); }
    Vec2f normalized() const {
        float l = length();
        return (l > 0.0001f) ? Vec2f(x / l, y / l) : Vec2f(0.0f, 0.0f);
    }
};

inline float clampf(float v, float minVal, float maxVal) {
    if (v < minVal) return minVal;
    if (v > maxVal) return maxVal;
    return v;
}

inline float lerpf(float a, float b, float t) {
    return a + (b - a) * clampf(t, 0.0f, 1.0f);
}

inline float smoothstep(float edge0, float edge1, float x) {
    float t = clampf((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float randFloat(float minVal, float maxVal) {
    return minVal + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxVal - minVal)));
}

} // namespace CosmicRobotics
