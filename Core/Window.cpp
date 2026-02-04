#include "Window.h"
#include "../Input/InputHandler.h"
#include "../Utils/Logger.h"
#include <windowsx.h>

namespace Core {

    Window::Window() 
        : m_hwnd(nullptr)
        , m_hInstance(nullptr)
        , m_width(0)
        , m_height(0)
        , m_className(L"PawliWindowClass")
        , m_inputHandler(nullptr)
        , m_timerId(0)
    {}

    Window::~Window() {
        Shutdown();
    }

    bool Window::Initialize(HINSTANCE hInstance, int width, int height) {
        m_hInstance = hInstance;
        m_width = width;
        m_height = height;

        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = m_className.c_str();
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);

        if (!RegisterClassEx(&wc)) {
            Utils::Logger::Log(L"Failed to register window class.");
            return false;
        }


        DWORD exStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
        DWORD style = WS_POPUP;

        m_hwnd = CreateWindowEx(
            exStyle,
            m_className.c_str(),
            L"Pawli",
            style,
            100, 100,
            width, height,
            nullptr,
            nullptr,
            hInstance,
            this
        );

        if (!m_hwnd) {
            Utils::Logger::Log(L"Failed to create window.");
            return false;
        }

        ShowWindow(m_hwnd, SW_SHOW);
        return true;
    }

    void Window::Shutdown() {
        if (m_timerId) {
            KillTimer(m_hwnd, m_timerId);
            m_timerId = 0;
        }
        if (m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
        UnregisterClass(m_className.c_str(), m_hInstance);
    }

    void Window::SetPosition(int x, int y) {
        if (m_hwnd) {
            SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }

    void Window::GetPosition(int& x, int& y) const {
        if (m_hwnd) {
            RECT rect;
            GetWindowRect(m_hwnd, &rect);
            x = rect.left;
            y = rect.top;
        }
    }

    void Window::SetTimerInterval(UINT intervalMs) {
        if (m_timerId) {
            KillTimer(m_hwnd, m_timerId);
        }
        m_timerId = SetTimer(m_hwnd, 1, intervalMs, nullptr);
    }

    bool Window::ProcessMessages() {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }

    LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        Window* pThis = nullptr;

        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (Window*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->m_hwnd = hwnd;
        } else {
            pThis = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if (pThis) {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        // Handle timer
        if (uMsg == WM_TIMER && wParam == 1) {
            if (m_timerCallback) {
                m_timerCallback();
            }
            return 0;
        }

        // Handle hit testing for click-through on transparent areas
        if (uMsg == WM_NCHITTEST) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(m_hwnd, &pt);
            
            if (m_hitTestCallback) {
                if (m_hitTestCallback(pt.x, pt.y)) {
                    // Opaque pixel - handle normally
                    return HTCLIENT;
                } else {
                    // Transparent pixel - click through
                    return HTTRANSPARENT;
                }
            }
            return HTCLIENT;
        }

        if (m_inputHandler) {
            switch (uMsg) {
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MOUSEMOVE:
                case WM_MOUSELEAVE:
                    m_inputHandler->HandleMessage(m_hwnd, uMsg, wParam, lParam);
                    break;
            }
        }

        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}
