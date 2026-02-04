#pragma once
#include <windows.h>
#include <functional>

namespace Input {

    enum class InputEventType {
        LeftClick,
        RightClick,
        DragStart,
        DragMove,
        DragEnd,
        MouseEnter,
        MouseLeave
    };

    struct InputEvent {
        InputEventType type;
        int x, y;           // Position relative to window
        int screenX, screenY; // Screen coordinates
    };

    using InputCallback = std::function<void(const InputEvent&)>;

    class InputHandler {
    public:
        InputHandler();
        ~InputHandler();

        void SetCallback(InputCallback callback);
        
        // Called from WindowProc
        bool HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // For alpha hit testing
        void SetHitTestCallback(std::function<bool(int, int)> callback);

        bool IsDragging() const { return m_isDragging; }

    private:
        InputCallback m_callback;
        std::function<bool(int, int)> m_hitTestCallback;
        bool m_isDragging;
        bool m_isMouseOver;
        POINT m_dragStartPos;
    };
}
