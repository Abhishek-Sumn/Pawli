#pragma once
#include <windows.h>

namespace Utils {

    class Timer {
    public:
        Timer() : m_lastTime(0), m_frequency(0), m_deltaTime(0) {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            m_frequency = (double)freq.QuadPart;
            
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            m_lastTime = now.QuadPart;
        }

        // Returns delta time in seconds
        float Tick() {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            
            m_deltaTime = (float)((now.QuadPart - m_lastTime) / m_frequency);
            m_lastTime = now.QuadPart;
            
            return m_deltaTime;
        }

        float GetDeltaTime() const { return m_deltaTime; }

    private:
        long long m_lastTime;
        double m_frequency;
        float m_deltaTime;
    };
}
