#pragma once
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>

#pragma comment(lib, "d2d1.lib")

namespace Assets {
    class SpriteAtlas;
    struct Frame;
}

namespace Core {
    class RenderContext {
    public:
        RenderContext();
        ~RenderContext();

        bool Initialize(HWND hwnd);
        void BeginDraw(HDC hdc = nullptr);
        void EndDraw();
        void Clear();

        void UpdateLayeredWindow(HWND hwnd, HDC hdcDest, int width, int height);

        void DrawSprite(ID2D1Bitmap* texture, const Assets::Frame* frame, float x, float y, bool flipX = false);
        void DrawBitmap(ID2D1Bitmap* bitmap, float x, float y, float width, float height);
        void DrawTestRect(float x, float y, float width, float height);

        ID2D1RenderTarget* GetRenderTarget() { return m_pRenderTarget; }
        ID2D1Factory* GetFactory() const { return m_pD2DFactory; }

    private:
        void CreateDeviceResources(HWND hwnd);
        void DiscardDeviceResources();

        HWND m_hwnd;
        ID2D1Factory* m_pD2DFactory;
        ID2D1DCRenderTarget* m_pRenderTarget;
        ID2D1SolidColorBrush* m_pBrush;

        HDC m_hMemoryDC;
        HBITMAP m_hBitmap;
        HBITMAP m_hOldBitmap;
        BYTE* m_pBitmapBits;
        int m_bitmapWidth;
        int m_bitmapHeight;
    };
}
