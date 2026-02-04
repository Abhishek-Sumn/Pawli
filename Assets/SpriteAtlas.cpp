#include "SpriteAtlas.h"

namespace Assets {

    SpriteAtlas::SpriteAtlas() : m_texture(nullptr), m_width(0), m_height(0) {}

    SpriteAtlas::~SpriteAtlas() {
        // Release all animation textures
        for (auto& pair : m_animations) {
            if (pair.second.texture) {
                pair.second.texture->Release();
            }
        }
        m_animations.clear();
        
        if (m_texture) {
            m_texture->Release();
            m_texture = nullptr;
        }
    }

    void SpriteAtlas::SetTexture(ID2D1Bitmap* texture) {
        if (m_texture) {
            m_texture->Release();
        }
        m_texture = texture;
    }

    void SpriteAtlas::AddAnimation(const std::string& name, const Animation& anim) {
        m_animations[name] = anim;
    }

    const Animation* SpriteAtlas::GetAnimation(const std::string& name) const {
        auto it = m_animations.find(name);
        if (it != m_animations.end()) {
            return &it->second;
        }
        return nullptr;
    }

    const Frame* SpriteAtlas::GetFrame(const std::string& animName, int frameIndex) const {
        const Animation* anim = GetAnimation(animName);
        if (anim && frameIndex >= 0 && frameIndex < (int)anim->frames.size()) {
            return &anim->frames[frameIndex];
        }
        return nullptr;
    }

    ID2D1Bitmap* SpriteAtlas::GetAnimationTexture(const std::string& animName) const {
        const Animation* anim = GetAnimation(animName);
        if (anim && anim->texture) {
            return anim->texture;
        }
        return m_texture; // Fallback to default
    }
}
