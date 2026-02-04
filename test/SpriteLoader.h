#pragma once
#include "SpriteAtlas.h"
#include <d2d1.h>
#include <wincodec.h>
#include <string>

#pragma comment(lib, "windowscodecs.lib")

namespace Assets {

    class SpriteLoader {
    public:
        SpriteLoader();
        ~SpriteLoader();

        bool Initialize();
        
        // Load a PNG file and create a D2D bitmap
        ID2D1Bitmap* LoadBitmap(ID2D1RenderTarget* renderTarget, const wchar_t* filePath);
        
        // Load kitty atlas with all animations
        SpriteAtlas* LoadKittyAtlas(ID2D1RenderTarget* renderTarget, const wchar_t* assetsFolder);

        // Create a simple test atlas with a colored rectangle
        SpriteAtlas* CreateTestAtlas(ID2D1RenderTarget* renderTarget);

    private:
        IWICImagingFactory* m_pWICFactory;
    };
}
