#include "InputHandler.h"
#include "../Utils/Logger.h"
#include <windowsx.h>

namespace Input {

    InputHandler::InputHandler() 
        : m_isDragging(false)
        , m_isMouseOver(false)
        , m_dragStartPos{ 0, 0 } 
    {}

    InputHandler::~InputHandler() {}

    void InputHandler::SetCallback(InputCallback callback) {
        m_callback = callback;
    }

    void InputHandler::SetHitTestCallback(std::function<bool(int, int)> callback) {
        m_hitTestCallback = callback;
    }

    bool InputHandler::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        InputEvent evt;
        evt.x = LOWORD(lParam);
        evt.y = HIWORD(lParam);
        
        POINT screenPt = { evt.x, evt.y };
        ClientToScreen(hwnd, &screenPt);
        evt.screenX = screenPt.x;
        evt.screenY = screenPt.y;

        switch (msg) {
            case WM_LBUTTONDOWN:
                evt.type = InputEventType::LeftClick;
                if (m_callback) m_callback(evt);
                return true;

            case WM_RBUTTONDOWN:
                evt.type = InputEventType::RightClick;
                m_isDragging = true;
                m_dragStartPos = { evt.x, evt.y };
                SetCapture(hwnd);
                
                evt.type = InputEventType::DragStart;
                if (m_callback) m_callback(evt);
                return true;

            case WM_RBUTTONUP:
                if (m_isDragging) {
                    m_isDragging = false;
                    ReleaseCapture();
                    evt.type = InputEventType::DragEnd;
                    if (m_callback) m_callback(evt);
                }
                return true;

            case WM_MOUSEMOVE:
                if (m_isDragging) {
                    evt.type = InputEventType::DragMove;
                    if (m_callback) m_callback(evt);
                }
                
                if (!m_isMouseOver) {
                    m_isMouseOver = true;
                    TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                    TrackMouseEvent(&tme);
                    
                    evt.type = InputEventType::MouseEnter;
                    if (m_callback) m_callback(evt);
                }
                return true;

            case WM_MOUSELEAVE:
                m_isMouseOver = false;
                evt.type = InputEventType::MouseLeave;
                if (m_callback) m_callback(evt);
                return true;

            case WM_NCHITTEST: {
                // Perform alpha hit test
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ScreenToClient(hwnd, &pt);
                
                if (m_hitTestCallback && m_hitTestCallback(pt.x, pt.y)) {
                    // Hit on opaque pixel
                    return false;
                }
                // Click-through on transparent area
                // Return HTTRANSPARENT through the lParam
                return true;
            }
        }

        return false;
    }
}
