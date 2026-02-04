#pragma once
#include "../Utils/Math.h"

namespace Pet {

    class PetPhysics {
    public:
        PetPhysics() 
            : m_easeSpeed(5.0f)
        {}

        // Smooth interpolation toward target
        Utils::Vec2 Ease(const Utils::Vec2& current, const Utils::Vec2& target, float dt) {
            float t = 1.0f - expf(-m_easeSpeed * dt);
            return Utils::Vec2::Lerp(current, target, t);
        }

        // Ease a single value
        float Ease(float current, float target, float dt) {
            float t = 1.0f - expf(-m_easeSpeed * dt);
            return Utils::Lerp(current, target, t);
        }

        void SetEaseSpeed(float speed) { m_easeSpeed = speed; }

    private:
        float m_easeSpeed;
    };
}
