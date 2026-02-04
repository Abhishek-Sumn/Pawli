#include "RenderContext.h"
#include "../Assets/SpriteAtlas.h"
#include "../Utils/Logger.h"

namespace Core {

    RenderContext::RenderContext() 
        : m_hwnd(nullptr)
        , m_pD2DFactory(nullptr)
        , m_pRenderTarget(nullptr)
        , m_pBrush(nullptr)
        , m_hMemoryDC(nullptr)
        , m_hBitmap(nullptr)
        , m_hOldBitmap(nullptr)
        , m_pBitmapBits(nullptr)
        , m_bitmapWidth(0)
        , m_bitmapHeight(0)
    {}

    RenderContext::~RenderContext() {
        DiscardDeviceResources();
        if (m_pD2DFactory) {
            m_pD2DFactory->Release();
            m_pD2DFactory = nullptr;
        }
    }

    bool RenderContext::Initialize(HWND hwnd) {
        m_hwnd = hwnd;
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
        if (FAILED(hr)) {
            Utils::Logger::Log(L"Failed to create D2D Factory.");
            return false;
        }
        
        CreateDeviceResources(hwnd);
        return m_pRenderTarget != nullptr;
    }

    void RenderContext::CreateDeviceResources(HWND hwnd) {
        if (!m_pRenderTarget) {
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );

            HRESULT hr = m_pD2DFactory->CreateDCRenderTarget(&props, &m_pRenderTarget);

            if (FAILED(hr)) {
                Utils::Logger::Log(L"Failed to create DCRenderTarget.");
                return;
            }

            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pBrush);
            
            // mem dc and bitmap for window size
            RECT rc;
            GetWindowRect(hwnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;

            HDC hdc = GetDC(hwnd);
            m_hMemoryDC = CreateCompatibleDC(hdc);
            
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w;
            bmi.bmiHeader.biHeight = -h; // Top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            
            m_hBitmap = CreateDIBSection(m_hMemoryDC, &bmi, DIB_RGB_COLORS, (void**)&m_pBitmapBits, nullptr, 0);
            m_hOldBitmap = (HBITMAP)SelectObject(m_hMemoryDC, m_hBitmap);
            m_bitmapWidth = w;
            m_bitmapHeight = h;
            
            ReleaseDC(hwnd, hdc);
        }
    }

    void RenderContext::DiscardDeviceResources() {
        if (m_hMemoryDC) {
            SelectObject(m_hMemoryDC, m_hOldBitmap);
            DeleteObject(m_hBitmap);
            DeleteDC(m_hMemoryDC);
            m_hMemoryDC = nullptr;
        }
        if (m_pBrush) {
            m_pBrush->Release();
            m_pBrush = nullptr;
        }
        if (m_pRenderTarget) {
            m_pRenderTarget->Release();
            m_pRenderTarget = nullptr;
        }
    }

    void RenderContext::BeginDraw(HDC hdc) {
        if (!m_pRenderTarget) {
            CreateDeviceResources(m_hwnd);
        }
        if (m_pRenderTarget) {
            // Clear the bitmap buffer to fully transparent before drawing
            if (m_pBitmapBits && m_bitmapWidth > 0 && m_bitmapHeight > 0) {
                memset(m_pBitmapBits, 0, m_bitmapWidth * m_bitmapHeight * 4);
            }
            
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            m_pRenderTarget->BindDC(m_hMemoryDC, &rc);
            m_pRenderTarget->BeginDraw();
        }
    }

    void RenderContext::EndDraw() {
        if (m_pRenderTarget) {
            HRESULT hr = m_pRenderTarget->EndDraw();
            if (hr == D2DERR_RECREATE_TARGET) {
                DiscardDeviceResources();
            } else {
                // Perform UpdateLayeredWindow
                RECT rc;
                GetWindowRect(m_hwnd, &rc);
                UpdateLayeredWindow(m_hwnd, nullptr, (rc.right - rc.left), (rc.bottom - rc.top));
            }
        }
    }

    void RenderContext::UpdateLayeredWindow(HWND hwnd, HDC hdcDest, int width, int height) {
        POINT ptSrc = { 0, 0 };
        SIZE size = { width, height };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        
        ::UpdateLayeredWindow(hwnd, hdcDest, nullptr, &size, m_hMemoryDC, &ptSrc, 0, &blend, ULW_ALPHA);
    }

    void RenderContext::Clear() {
        if (m_pRenderTarget) {
            m_pRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }

    void RenderContext::DrawSprite(ID2D1Bitmap* texture, const Assets::Frame* frame, float x, float y, bool flipX) {
        if (!m_pRenderTarget || !texture || !frame) return;

        // Target size - scale to fit within this while preserving aspect ratio
        const float maxSize = 150.0f;
        
        float scale = 1.0f;
        if (frame->width > frame->height) {
            scale = maxSize / frame->width;
        } else {
            scale = maxSize / frame->height;
        }
        
        float drawWidth = frame->width * scale;
        float drawHeight = frame->height * scale;
        
        float offsetX = (maxSize - drawWidth) / 2.0f;
        float offsetY = (maxSize - drawHeight) / 2.0f;

        D2D1_RECT_F srcRect = D2D1::RectF(frame->x, frame->y, frame->x + frame->width, frame->y + frame->height);
        D2D1_RECT_F destRect = D2D1::RectF(x + offsetX, y + offsetY, x + offsetX + drawWidth, y + offsetY + drawHeight);

        if (flipX) {
            float mid = x + maxSize / 2.0f;
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(mid, 0)));
        }

        m_pRenderTarget->DrawBitmap(texture, destRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
        
        if (flipX) {
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        }
    }

    void RenderContext::DrawBitmap(ID2D1Bitmap* bitmap, float x, float y, float width, float height) {
        if (!m_pRenderTarget || !bitmap) return;
        D2D1_RECT_F destRect = D2D1::RectF(x, y, x + width, y + height);
        m_pRenderTarget->DrawBitmap(bitmap, destRect);
    }

    void RenderContext::DrawTestRect(float x, float y, float width, float height) {
        if (m_pRenderTarget && m_pBrush) {
            D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
            m_pRenderTarget->FillRectangle(rect, m_pBrush);
        }
    }
}
