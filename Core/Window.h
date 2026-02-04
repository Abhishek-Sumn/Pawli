#pragma once
#include <windows.h>
#include <string>
#include <functional>

namespace Input {
    class InputHandler;
}

namespace Core {
    class Window {
    public:
        Window();
        ~Window();

        bool Initialize(HINSTANCE hInstance, int width, int height);
        void Shutdown();
        bool ProcessMessages();
        
        HWND GetHwnd() const { return m_hwnd; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        void SetPosition(int x, int y);
        void GetPosition(int& x, int& y) const;

        // Input handler integration
        void SetInputHandler(Input::InputHandler* handler) { m_inputHandler = handler; }

        // Hit test callback for alpha testing
        void SetHitTestCallback(std::function<bool(int, int)> callback) { m_hitTestCallback = callback; }

        void SetTimerInterval(UINT intervalMs);

        void SetTimerCallback(std::function<void()> callback) { m_timerCallback = callback; }

    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        
        HWND m_hwnd;
        HINSTANCE m_hInstance;
        int m_width;
        int m_height;
        std::wstring m_className;
        Input::InputHandler* m_inputHandler;
        std::function<bool(int, int)> m_hitTestCallback;
        std::function<void()> m_timerCallback;
        UINT_PTR m_timerId;
    };
}
