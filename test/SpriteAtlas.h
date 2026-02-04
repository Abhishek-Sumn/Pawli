#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <d2d1.h>

namespace Assets {

    struct Frame {
        float x, y, width, height;
    };

    struct Animation {
        std::vector<Frame> frames;
        float frameDuration; // seconds per frame
        ID2D1Bitmap* texture; // Texture for this animation
    };

    class SpriteAtlas {
    public:
        SpriteAtlas();
        ~SpriteAtlas();

        void SetTexture(ID2D1Bitmap* texture);
        ID2D1Bitmap* GetTexture() const { return m_texture; }

        void AddAnimation(const std::string& name, const Animation& anim);
        const Animation* GetAnimation(const std::string& name) const;
        const Frame* GetFrame(const std::string& animName, int frameIndex) const;
        
        // Get texture for specific animation
        ID2D1Bitmap* GetAnimationTexture(const std::string& animName) const;

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        void SetSize(int w, int h) { m_width = w; m_height = h; }

    private:
        ID2D1Bitmap* m_texture;
        std::unordered_map<std::string, Animation> m_animations;
        int m_width;
        int m_height;
    };
}
