#pragma once
#include <windows.h>
#include <vector>

namespace Desktop {

    struct MonitorInfo {
        HMONITOR handle;
        RECT bounds;        
        RECT workArea;      // Usable area (excluding taskbar)
        UINT dpiX;
        UINT dpiY;
        bool isPrimary;
    };

    class MonitorManager {
    public:
        MonitorManager();
        ~MonitorManager();

        void Refresh();
        
        const MonitorInfo* GetMonitorFromPoint(POINT pt) const;
        const MonitorInfo* GetMonitorFromWindow(HWND hwnd) const;
        const MonitorInfo* GetPrimaryMonitor() const;
        int GetMonitorIndex(POINT pt) const;  // Returns -1 if not found
        
        const std::vector<MonitorInfo>& GetAllMonitors() const { return m_monitors; }
        size_t GetMonitorCount() const { return m_monitors.size(); }

    private:
        static BOOL CALLBACK EnumMonitorsCallback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
        
        std::vector<MonitorInfo> m_monitors;
    };
}
