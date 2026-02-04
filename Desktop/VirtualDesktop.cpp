#include "VirtualDesktop.h"
#include "../Utils/Logger.h"

namespace Desktop {

    VirtualDesktop::VirtualDesktop() : m_pDesktopManager(nullptr) {}

    VirtualDesktop::~VirtualDesktop() {
        if (m_pDesktopManager) {
            m_pDesktopManager->Release();
            m_pDesktopManager = nullptr;
        }
    }

    bool VirtualDesktop::Initialize() {
        HRESULT hr = CoCreateInstance(
            CLSID_VirtualDesktopManager,
            nullptr,
            CLSCTX_ALL,
            IID_PPV_ARGS(&m_pDesktopManager)
        );

        if (FAILED(hr)) {
            Utils::Logger::Log(L"VirtualDesktop: Failed to create IVirtualDesktopManager (hr=0x%08X)", hr);
            return false;
        }

        Utils::Logger::Log(L"VirtualDesktop: Initialized successfully");
        return true;
    }

    bool VirtualDesktop::IsWindowOnCurrentDesktop(HWND hwnd) const {
        if (!m_pDesktopManager || !hwnd) {
            return true; // Assume yes if not available
        }

        BOOL isOnCurrent = FALSE;
        HRESULT hr = m_pDesktopManager->IsWindowOnCurrentVirtualDesktop(hwnd, &isOnCurrent);
        
        if (FAILED(hr)) {
            return true; // Assume yes on failure
        }

        return isOnCurrent == TRUE;
    }

    bool VirtualDesktop::IsCurrentDesktopActive() const {
        // The public API doesn't directly tell us if the desktop is active,
        // but we can infer it from window visibility
        return true; // Placeholder
    }
}
