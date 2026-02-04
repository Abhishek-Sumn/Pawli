#include "App.h"
#include "EventBus.h"
#include "../Utils/Logger.h"

namespace Core {

    App::App() 
        : m_atlas(nullptr)
        , m_dragOffsetX(0)
        , m_dragOffsetY(0)
    {}

    App::~App() {
        if (m_atlas) {
            delete m_atlas;
            m_atlas = nullptr;
        }
    }

    bool App::Initialize(HINSTANCE hInstance, int cmdShow) {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) {
            Utils::Logger::Log(L"Failed to initialize COM.");
            return false;
        }

        if (!m_window.Initialize(hInstance, 200, 200)) {
            return false;
        }

        if (!m_renderContext.Initialize(m_window.GetHwnd())) {
            return false;
        }

        if (!m_spriteLoader.Initialize()) {
            Utils::Logger::Log(L"Failed to initialize sprite loader.");
            return false;
        }

        // Exec dir find wirh asset discovery
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring exeDir(exePath);
        size_t lastSlash = exeDir.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            exeDir = exeDir.substr(0, lastSlash);
        }
        
        std::wstring assetsPath = exeDir + L"\\Assets";
        m_atlas = m_spriteLoader.LoadKittyAtlas(m_renderContext.GetRenderTarget(), assetsPath.c_str());
        
        if (!m_atlas || !m_atlas->GetAnimation("idle")) {
            assetsPath = exeDir + L"\\..\\Assets";
            if (m_atlas) delete m_atlas;
            m_atlas = m_spriteLoader.LoadKittyAtlas(m_renderContext.GetRenderTarget(), assetsPath.c_str());
        }
        
        if (!m_atlas || !m_atlas->GetAnimation("idle")) {
            Utils::Logger::Log(L"Kitty atlas not found, using test atlas.");
            if (m_atlas) delete m_atlas;
            m_atlas = m_spriteLoader.CreateTestAtlas(m_renderContext.GetRenderTarget());
        }

        if (!m_atlas) {
            Utils::Logger::Log(L"Failed to create any atlas.");
            return false;
        }

        m_animator.SetAtlas(m_atlas);
        m_animator.Play("idle", true);

        m_desktopTracker.Initialize(m_window.GetHwnd());
        
        EventBus::Get().Subscribe(EventType::MonitorChanged, [this](const Event& evt) {
            UpdateMovementBounds();
        });

        UpdateMovementBounds();
        m_petController.Initialize();

        m_inputHandler.SetCallback([this](const Input::InputEvent& evt) {
            OnInput(evt);
        });
        m_window.SetInputHandler(&m_inputHandler);

        m_window.SetHitTestCallback([this](int x, int y) {
            return HitTest(x, y);
        });

        m_window.SetTimerCallback([this]() {
            OnRender();
        });
        m_window.SetTimerInterval(33);

        UpdateWindowPosition();

        Utils::Logger::Log(L"Pawli initialized with kitty sprites!");
        return true;
    }

    void App::Run() {
        while (m_window.ProcessMessages()) {
            Sleep(1);
        }
        CoUninitialize();
    }

    void App::OnDisplayChange() {
        Utils::Logger::Log(L"Display configuration changed");
        m_desktopTracker.RefreshMonitors();
        UpdateMovementBounds();
    }

    void App::OnRender() {
        m_desktopTracker.Update();

        float dt = m_timer.Tick();
        m_petController.Update(dt);

        // Update animation based on pet state
        UpdateAnimation();
        m_animator.Update(dt);

        UpdateWindowPosition();

        m_renderContext.BeginDraw();
        m_renderContext.Clear();

        const Assets::Frame* frame = m_animator.GetCurrentFrame();
        if (frame) {
            std::string animName = m_animator.GetCurrentAnimation();
            ID2D1Bitmap* texture = m_atlas->GetAnimationTexture(animName);
            if (texture) {
                bool flipX = (m_petController.GetContext().direction == Pet::PetDirection::Left);
                m_renderContext.DrawSprite(texture, frame, 0, 0, flipX);
            }
        }

        m_renderContext.EndDraw();
    }

    void App::UpdateAnimation() {
        const Pet::PetContext& ctx = m_petController.GetContext();
        
        if (!ctx.currentAnimation.empty()) {
            bool loop = (ctx.currentAnimation != "react"); 
            m_animator.Play(ctx.currentAnimation, loop);
        } else {
            m_animator.Play("idle", true);
        }
    }

    void App::UpdateWindowPosition() {
        const Pet::PetContext& ctx = m_petController.GetContext();
        m_window.SetPosition((int)ctx.position.x, (int)ctx.position.y);
    }

    void App::UpdateMovementBounds() {
        // Use virtual screen bounds (spans ALL monitors)
        RECT virtualScreen;
        virtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        virtualScreen.right = virtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        virtualScreen.bottom = virtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
        
        // Small margin to keep kitty from going off-screen edges
        virtualScreen.left += 50;
        virtualScreen.top += 50;
        virtualScreen.right -= 250;   // Account for sprite width + margin
        virtualScreen.bottom -= 250;  // Account for sprite height + taskbar
        
        m_petController.SetMovementBounds(virtualScreen);
        Utils::Logger::Log(L"Multi-display bounds: (%d,%d) to (%d,%d)", 
            virtualScreen.left, virtualScreen.top, virtualScreen.right, virtualScreen.bottom);
    }

    void App::OnInput(const Input::InputEvent& evt) {
        switch (evt.type) {
            case Input::InputEventType::LeftClick:
                m_petController.OnLeftClick();
                m_animator.Play("react", false);
                break;

            case Input::InputEventType::DragStart:
                m_dragOffsetX = evt.x;
                m_dragOffsetY = evt.y;
                m_petController.OnRightClickStart();
                break;

            case Input::InputEventType::DragMove: {
                int newX = evt.screenX - m_dragOffsetX;
                int newY = evt.screenY - m_dragOffsetY;
                m_petController.OnDrag((float)newX, (float)newY);
                UpdateWindowPosition();
                break;
            }

            case Input::InputEventType::DragEnd:
                m_petController.OnRightClickEnd();
                UpdateMovementBounds();
                break;

            default:
                break;
        }
    }

    bool App::HitTest(int x, int y) {
        const Assets::Frame* frame = m_animator.GetCurrentFrame();
        if (frame) {
            if (x >= 0 && x < (int)frame->width && y >= 0 && y < (int)frame->height) {
                return true;
            }
        }
        return false;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Core::App app;
    if (app.Initialize(hInstance, nCmdShow)) {
        app.Run();
    }
    return 0;
}
