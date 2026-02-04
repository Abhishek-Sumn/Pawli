#pragma once
#include "MonitorManager.h"
#include "VirtualDesktop.h"
#include "../Core/EventBus.h"
#include <windows.h>
#include <functional>

namespace Desktop {

    class DesktopTracker {
    public:
        DesktopTracker();
        ~DesktopTracker();

        bool Initialize(HWND petWindow);
        void Update();

        const MonitorInfo* GetCurrentMonitor() const { return m_currentMonitor; }
        bool IsOnCurrentDesktop() const;

        POINT GetCursorPosition() const;

        RECT GetCurrentWorkArea() const;

        void RefreshMonitors();

    private:
        void CheckMonitorChange();
        void CheckDesktopChange();

        MonitorManager m_monitorManager;
        VirtualDesktop m_virtualDesktop;
        HWND m_petWindow;
        const MonitorInfo* m_currentMonitor;
        HMONITOR m_lastMonitor;
        bool m_wasOnDesktop;
    };
}
