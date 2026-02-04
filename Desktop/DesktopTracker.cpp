#include "DesktopTracker.h"
#include "../Utils/Logger.h"

namespace Desktop {

    DesktopTracker::DesktopTracker()
        : m_petWindow(nullptr)
        , m_currentMonitor(nullptr)
        , m_lastMonitor(nullptr)
        , m_wasOnDesktop(true)
    {}

    DesktopTracker::~DesktopTracker() {}

    bool DesktopTracker::Initialize(HWND petWindow) {
        m_petWindow = petWindow;
        
        m_virtualDesktop.Initialize();
        
        // Find current monitor
        m_currentMonitor = m_monitorManager.GetMonitorFromWindow(petWindow);
        if (m_currentMonitor) {
            m_lastMonitor = m_currentMonitor->handle;
        }

        Utils::Logger::Log(L"DesktopTracker: Initialized");
        return true;
    }

    void DesktopTracker::Update() {
        CheckMonitorChange();
        CheckDesktopChange();
    }

    void DesktopTracker::CheckMonitorChange() {
        if (!m_petWindow) return;

        HMONITOR currentHandle = MonitorFromWindow(m_petWindow, MONITOR_DEFAULTTONEAREST);
        
        if (currentHandle != m_lastMonitor) {
            m_lastMonitor = currentHandle;
            m_currentMonitor = m_monitorManager.GetMonitorFromWindow(m_petWindow);
            
            Utils::Logger::Log(L"DesktopTracker: Monitor changed");
            
            // Emit monitor changed event
            Core::Event evt;
            evt.type = Core::EventType::MonitorChanged;
            evt.ptrParam = (void*)m_currentMonitor;
            Core::EventBus::Get().Emit(evt);
        }
    }

    void DesktopTracker::CheckDesktopChange() {
        if (!m_petWindow) return;

        bool isOnDesktop = m_virtualDesktop.IsWindowOnCurrentDesktop(m_petWindow);
        
        if (isOnDesktop != m_wasOnDesktop) {
            m_wasOnDesktop = isOnDesktop;
            
            Utils::Logger::Log(L"DesktopTracker: Desktop changed, on current: %s", 
                isOnDesktop ? L"Yes" : L"No");
            
            Core::Event evt;
            evt.type = Core::EventType::DesktopChanged;
            evt.intParam1 = isOnDesktop ? 1 : 0;
            Core::EventBus::Get().Emit(evt);
        }
    }

    bool DesktopTracker::IsOnCurrentDesktop() const {
        return m_virtualDesktop.IsWindowOnCurrentDesktop(m_petWindow);
    }

    POINT DesktopTracker::GetCursorPosition() const {
        POINT pt;
        GetCursorPos(&pt);
        return pt;
    }

    RECT DesktopTracker::GetCurrentWorkArea() const {
        if (m_currentMonitor) {
            return m_currentMonitor->workArea;
        }
        // Fallback to primary
        RECT workArea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
        return workArea;
    }

    void DesktopTracker::RefreshMonitors() {
        m_monitorManager.Refresh();
        m_currentMonitor = m_monitorManager.GetMonitorFromWindow(m_petWindow);
    }
}
