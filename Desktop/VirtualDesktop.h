#pragma once
#include <windows.h>
#include <shobjidl.h>

namespace Desktop {

    class VirtualDesktop {
    public:
        VirtualDesktop();
        ~VirtualDesktop();

        bool Initialize();
        
        // Check if window is on current virtual desktop
        bool IsWindowOnCurrentDesktop(HWND hwnd) const;
        
        bool IsCurrentDesktopActive() const;

    private:
        IVirtualDesktopManager* m_pDesktopManager;
    };
}
