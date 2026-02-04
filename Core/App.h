#pragma once
#include <windows.h>
#include "Window.h"
#include "RenderContext.h"
#include "../Input/InputHandler.h"
#include "../Assets/SpriteLoader.h"
#include "../Assets/SpriteAtlas.h"
#include "../Pet/PetController.h"
#include "../Pet/PetAnimator.h"
#include "../Desktop/DesktopTracker.h"
#include "../Utils/Timer.h"
#include "../Utils/Math.h"

namespace Core {
    class App {
    public:
        App();
        ~App();

        bool Initialize(HINSTANCE hInstance, int cmdShow);
        void Run();

        void OnDisplayChange();

    private:
        void OnRender();
        void OnInput(const Input::InputEvent& evt);
        bool HitTest(int x, int y);
        void UpdateWindowPosition();
        void UpdateMovementBounds();
        void UpdateAnimation();

        Window m_window;
        RenderContext m_renderContext;
        Input::InputHandler m_inputHandler;
        Assets::SpriteLoader m_spriteLoader;
        Assets::SpriteAtlas* m_atlas;
        Utils::Timer m_timer;
        Pet::PetController m_petController;
        Pet::PetAnimator m_animator;
        Desktop::DesktopTracker m_desktopTracker;

        int m_dragOffsetX, m_dragOffsetY;
    };
}
