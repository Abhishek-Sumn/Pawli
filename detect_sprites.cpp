#include <windows.h>
#include <wincodec.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

void DetectSprite(const wchar_t* filename) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    IWICImagingFactory* pFactory = nullptr;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
    
    IWICBitmapDecoder* pDecoder = nullptr;
    hr = pFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
    
    IWICBitmapFrameDecode* pFrame = nullptr;
    hr = pDecoder->GetFrame(0, &pFrame);
    
    UINT width, height;
    pFrame->GetSize(&width, &height);
    
    IWICFormatConverter* pConverter = nullptr;
    pFactory->CreateFormatConverter(&pConverter);
    pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
    
    std::vector<BYTE> buffer(width * height * 4);
    pConverter->CopyPixels(nullptr, width * 4, (UINT)buffer.size(), buffer.data());
    
    UINT minX = width, minY = height, maxX = 0, maxY = 0;
    bool found = false;
    for (UINT y = 0; y < height; y++) {
        for (UINT x = 0; x < width; x++) {
            BYTE alpha = buffer[(y * width + x) * 4 + 3];
            if (alpha > 10) { // Some threshold for non-transparent
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
                found = true;
            }
        }
    }
    
    if (found) {
        std::wcout << filename << L" Content Rect: " << minX << L", " << minY << L", " << (maxX - minX + 1) << L", " << (maxY - minY + 1) << std::endl;
    } else {
        std::wcout << filename << L" No content found." << std::endl;
    }
    
    pConverter->Release();
    pFrame->Release();
    pDecoder->Release();
    pFactory->Release();
    CoUninitialize();
}

int main() {
    DetectSprite(L"Assets\\kitty_idle.png");
    DetectSprite(L"Assets\\kitty_walk.png");
    DetectSprite(L"Assets\\kitty_sleep.png");
    DetectSprite(L"Assets\\kitty_react.png");
    return 0;
}
