#pragma once
#include "../Assets/SpriteAtlas.h"
#include <string>
#include <unordered_map>
#include <d2d1.h>

namespace Pet {

    class PetAnimator {
    public:
        PetAnimator();
        ~PetAnimator();

        void SetAtlas(Assets::SpriteAtlas* atlas) { m_atlas = atlas; }
        
        // Set current animation
        void Play(const std::string& animationName, bool loop = true);
        
        // Update animation (call every frame)
        void Update(float deltaTime);
        
        // Get current frame for rendering
        const Assets::Frame* GetCurrentFrame() const;
        
        // Check if animation finished (for non-looping)
        bool IsComplete() const { return m_complete; }
        
        // Get current animation name
        const std::string& GetCurrentAnimation() const { return m_currentAnimation; }

    private:
        Assets::SpriteAtlas* m_atlas;
        std::string m_currentAnimation;
        int m_currentFrame;
        float m_elapsed;
        bool m_loop;
        bool m_complete;
    };
}
