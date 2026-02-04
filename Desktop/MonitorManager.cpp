#include "MonitorManager.h"
#include "../Utils/Logger.h"
#include <shellscalingapi.h>

#pragma comment(lib, "shcore.lib")

namespace Desktop {

    MonitorManager::MonitorManager() {
        Refresh();
    }

    MonitorManager::~MonitorManager() {}

    void MonitorManager::Refresh() {
        m_monitors.clear();
        EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsCallback, (LPARAM)this);
        Utils::Logger::Log(L"MonitorManager: Found %d monitors", (int)m_monitors.size());
    }

    BOOL CALLBACK MonitorManager::EnumMonitorsCallback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
        MonitorManager* self = (MonitorManager*)dwData;
        
        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        
        if (GetMonitorInfo(hMonitor, &monitorInfo)) {
            MonitorInfo info;
            info.handle = hMonitor;
            info.bounds = monitorInfo.rcMonitor;
            info.workArea = monitorInfo.rcWork;
            info.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
            
            // Get DPI
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
                info.dpiX = dpiX;
                info.dpiY = dpiY;
            } else {
                info.dpiX = 96;
                info.dpiY = 96;
            }
            
            self->m_monitors.push_back(info);
            
            Utils::Logger::Log(L"  Monitor: (%d, %d) - (%d, %d), DPI: %d, Primary: %s",
                info.bounds.left, info.bounds.top, info.bounds.right, info.bounds.bottom,
                info.dpiX, info.isPrimary ? L"Yes" : L"No");
        }
        
        return TRUE;
    }

    const MonitorInfo* MonitorManager::GetMonitorFromPoint(POINT pt) const {
        HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        for (const auto& monitor : m_monitors) {
            if (monitor.handle == hMonitor) {
                return &monitor;
            }
        }
        return nullptr;
    }

    const MonitorInfo* MonitorManager::GetMonitorFromWindow(HWND hwnd) const {
        HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        for (const auto& monitor : m_monitors) {
            if (monitor.handle == hMonitor) {
                return &monitor;
            }
        }
        return nullptr;
    }

    const MonitorInfo* MonitorManager::GetPrimaryMonitor() const {
        for (const auto& monitor : m_monitors) {
            if (monitor.isPrimary) {
                return &monitor;
            }
        }
        return m_monitors.empty() ? nullptr : &m_monitors[0];
    }

    int MonitorManager::GetMonitorIndex(POINT pt) const {
        HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        for (size_t i = 0; i < m_monitors.size(); i++) {
            if (m_monitors[i].handle == hMonitor) {
                return (int)i;
            }
        }
        return -1;
    }
}
