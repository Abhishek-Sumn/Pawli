#include "PetAnimator.h"
#include "../Utils/Logger.h"

namespace Pet {

    PetAnimator::PetAnimator()
        : m_atlas(nullptr)
        , m_currentFrame(0)
        , m_elapsed(0)
        , m_loop(true)
        , m_complete(false)
    {}

    PetAnimator::~PetAnimator() {}

    void PetAnimator::Play(const std::string& animationName, bool loop) {
        if (m_currentAnimation != animationName) {
            m_currentAnimation = animationName;
            m_currentFrame = 0;
            m_elapsed = 0;
            m_loop = loop;
            m_complete = false;
        }
    }

    void PetAnimator::Update(float deltaTime) {
        if (!m_atlas || m_currentAnimation.empty() || m_complete) return;

        const Assets::Animation* anim = m_atlas->GetAnimation(m_currentAnimation);
        if (!anim || anim->frames.empty()) return;

        m_elapsed += deltaTime;

        if (m_elapsed >= anim->frameDuration) {
            m_elapsed -= anim->frameDuration;
            m_currentFrame++;

            if (m_currentFrame >= (int)anim->frames.size()) {
                if (m_loop) {
                    m_currentFrame = 0;
                } else {
                    m_currentFrame = (int)anim->frames.size() - 1;
                    m_complete = true;
                }
            }
        }
    }

    const Assets::Frame* PetAnimator::GetCurrentFrame() const {
        if (!m_atlas || m_currentAnimation.empty()) return nullptr;
        return m_atlas->GetFrame(m_currentAnimation, m_currentFrame);
    }
}
