#include "SpriteLoader.h"
#include "../Utils/Logger.h"
#include <fstream>
#include <sstream>

namespace Assets {

    SpriteLoader::SpriteLoader() : m_pWICFactory(nullptr) {}

    SpriteLoader::~SpriteLoader() {
        if (m_pWICFactory) {
            m_pWICFactory->Release();
            m_pWICFactory = nullptr;
        }
    }

    bool SpriteLoader::Initialize() {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pWICFactory)
        );
        
        if (FAILED(hr)) {
            Utils::Logger::Log(L"Failed to create WIC factory");
            return false;
        }
        return true;
    }

    ID2D1Bitmap* SpriteLoader::LoadBitmap(ID2D1RenderTarget* renderTarget, const wchar_t* filePath) {
        if (!m_pWICFactory || !renderTarget) return nullptr;

        IWICBitmapDecoder* pDecoder = nullptr;
        IWICBitmapFrameDecode* pFrame = nullptr;
        IWICFormatConverter* pConverter = nullptr;

        HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
            filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

        if (SUCCEEDED(hr)) hr = pDecoder->GetFrame(0, &pFrame);
        if (SUCCEEDED(hr)) hr = m_pWICFactory->CreateFormatConverter(&pConverter);
        if (SUCCEEDED(hr)) {
            hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, 
                WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
        }

        if (FAILED(hr)) {
            if (pConverter) pConverter->Release();
            if (pFrame) pFrame->Release();
            if (pDecoder) pDecoder->Release();
            return nullptr;
        }

        UINT width, height;
        pFrame->GetSize(&width, &height);
        
        // Copy pixels to modify them (color keying)
        std::vector<BYTE> buffer(width * height * 4);
        pConverter->CopyPixels(nullptr, width * 4, (UINT)buffer.size(), buffer.data());

        // Simple color keying: treat near-white as transparent
        // Also detect actual content bounds to fix framing
        for (UINT i = 0; i < width * height; i++) {
            BYTE* pixel = &buffer[i * 4];
            BYTE b = pixel[0];
            BYTE g = pixel[1];
            BYTE r = pixel[2];
            
            // Simple color keying: treat near-green as transparent
            if (g > 200 && r < 100 && b < 100) {
                pixel[3] = 0;
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
            }
        }

        ID2D1Bitmap* pBitmap = nullptr;
        hr = renderTarget->CreateBitmap(
            D2D1::SizeU(width, height),
            buffer.data(),
            width * 4,
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &pBitmap
        );

        pConverter->Release();
        pFrame->Release();
        pDecoder->Release();

        if (FAILED(hr)) {
            Utils::Logger::Log(L"Failed to create bitmap from modified buffer: %s", filePath);
            return nullptr;
        }

        Utils::Logger::Log(L"Loaded and color-keyed bitmap: %s", filePath);
        return pBitmap;
    }

    SpriteAtlas* SpriteLoader::LoadKittyAtlas(ID2D1RenderTarget* renderTarget, const wchar_t* assetsFolder) {
        SpriteAtlas* atlas = new SpriteAtlas();
        
        std::wstring basePath(assetsFolder);
        if (!basePath.empty() && basePath.back() != L'\\' && basePath.back() != L'/') {
            basePath += L"\\";
        }

        auto LoadAnimation = [&](const std::wstring& filename, const std::string& animName, int frameCount, float duration, bool isGrid = false) {
            // We need the raw pixels to detect bounds, so we'll load manually here
            if (!m_pWICFactory || !renderTarget) return;

            IWICBitmapDecoder* pDecoder = nullptr;
            IWICBitmapFrameDecode* pFrame = nullptr;
            IWICFormatConverter* pConverter = nullptr;

            HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
                (basePath + filename).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

            if (SUCCEEDED(hr)) hr = pDecoder->GetFrame(0, &pFrame);
            if (SUCCEEDED(hr)) hr = m_pWICFactory->CreateFormatConverter(&pConverter);
            if (SUCCEEDED(hr)) {
                hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, 
                    WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
            }

            if (FAILED(hr)) {
                if (pConverter) pConverter->Release();
                if (pFrame) pFrame->Release();
                if (pDecoder) pDecoder->Release();
                return;
            }

            UINT width, height;
            pFrame->GetSize(&width, &height);
            std::vector<BYTE> buffer(width * height * 4);
            pConverter->CopyPixels(nullptr, width * 4, (UINT)buffer.size(), buffer.data());

            // Color keying (Green AND Grey/White checkerboard to Transparent)
            for (UINT i = 0; i < width * height; i++) {
                BYTE* pixel = &buffer[i * 4];
                BYTE b = pixel[0];
                BYTE g = pixel[1]; 
                BYTE r = pixel[2];
                
                // Aggressive green screen removal - catches all green shades
                if (g > 60 && g > r * 1.1 && g > b * 1.1) {
                    pixel[3] = 0; pixel[0] = 0; pixel[1] = 0; pixel[2] = 0;
                }
            }

            Animation anim;
            anim.frameDuration = duration;

            int rows = isGrid ? 2 : 1;
            int cols = isGrid ? 2 : frameCount;
            float frameW = (float)width / (float)cols;
            float frameH = (float)height / (float)rows;
            
            // Inset frames by 4 pixels on each edge to avoid border artifacts
            const float inset = 4.0f;

            Utils::Logger::Log(L"Animation %S: texture %dx%d, %d cols x %d rows, frame size %.0fx%.0f", 
                animName.c_str(), width, height, cols, rows, frameW, frameH);

            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    if (anim.frames.size() >= (size_t)frameCount) break;
                    
                    float fx = (float)c * frameW + inset;
                    float fy = (float)r * frameH + inset;
                    float fw = frameW - inset * 2;
                    float fh = frameH - inset * 2;
                    anim.frames.push_back({ fx, fy, fw, fh });
                    
                    Utils::Logger::Log(L"  Frame %d: x=%.0f y=%.0f w=%.0f h=%.0f", 
                        (int)anim.frames.size() - 1, fx, fy, fw, fh);
                }
            }

            // Create final bitmap from modified buffer
            hr = renderTarget->CreateBitmap(D2D1::SizeU(width, height), buffer.data(), width * 4,
                D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), 
                &anim.texture);

            pConverter->Release(); pFrame->Release(); pDecoder->Release();

            if (SUCCEEDED(hr)) {
                atlas->AddAnimation(animName, anim);
                Utils::Logger::Log(L"Added animation %S: %d frames", animName.c_str(), (int)anim.frames.size());
            }
        };

        // v5/v6/v7 sprites with beautiful anime eyes for emotional connection
        LoadAnimation(L"kitty_idle_v5.png", "idle", 4, 0.3f, true);
        LoadAnimation(L"kitty_walk_v5.png", "walk", 4, 0.12f, true);
        LoadAnimation(L"kitty_sleep_v8.png", "sleep", 4, 0.5f, true);
        LoadAnimation(L"kitty_react_v6.png", "react", 4, 0.15f, true);
        
        // New variety animations for random actions
        LoadAnimation(L"kitty_yawn.png", "yawn", 4, 0.4f, true);
        LoadAnimation(L"kitty_love.png", "love", 4, 0.2f, true);
        LoadAnimation(L"kitty_curious.png", "curious", 4, 0.25f, true);
        // User-generated jump sprite (2x2 grid format)
        LoadAnimation(L"kitty_jump.png", "jump", 4, 0.12f, true);  // Display crossing jump, true for grid

        return atlas;
    }

    SpriteAtlas* SpriteLoader::CreateTestAtlas(ID2D1RenderTarget* renderTarget) {
        D2D1_SIZE_U size = D2D1::SizeU(64, 64);
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

        std::vector<UINT32> pixels(64 * 64);
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 64; x++) {
                pixels[y * 64 + x] = 0xFFFF0000;
            }
        }

        ID2D1Bitmap* pBitmap = nullptr;
        HRESULT hr = renderTarget->CreateBitmap(size, pixels.data(), 64 * 4, props, &pBitmap);

        if (FAILED(hr)) {
            Utils::Logger::Log(L"Failed to create test bitmap");
            return nullptr;
        }

        SpriteAtlas* atlas = new SpriteAtlas();
        atlas->SetTexture(pBitmap);
        atlas->SetSize(64, 64);

        Animation idleAnim;
        idleAnim.frameDuration = 1.0f;
        idleAnim.texture = pBitmap;
        idleAnim.frames.push_back({ 0, 0, 64, 64 });
        atlas->AddAnimation("idle", idleAnim);

        return atlas;
    }
}
