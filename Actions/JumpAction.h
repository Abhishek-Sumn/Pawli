#pragma once
#include "Action.h"
#include <cmath>

namespace Actions {

    class JumpAction : public Action {
    public:
        JumpAction()
            : m_duration(0.6f)      // Slightly longer for a big jump
            , m_elapsed(0)
            , m_complete(false)
            , m_startY(0)
            , m_startX(0)
            , m_jumpHeight(120.0f)   // Much higher jump
            , m_jumpWidth(250.0f)    // Significant horizontal distance
            , m_direction(1.0f)
        {}

        void OnEnter(Pet::PetContext& ctx) override {
            ctx.currentAnimation = "jump";
            ctx.mood = Pet::PetMood::Happy;
            m_elapsed = 0;
            m_complete = false;
            m_startY = ctx.position.y;
            m_startX = ctx.position.x;
            
            // Determine jump direction based on current facing
            m_direction = (ctx.direction == Pet::PetDirection::Right) ? 1.0f : -1.0f;
        }

        void OnUpdate(Pet::PetContext& ctx, float deltaTime) override {
            m_elapsed += deltaTime;
            float progress = m_elapsed / m_duration;
            
            if (progress > 1.0f) progress = 1.0f;
            
            // vertical arc (sine wave)
            float jumpOffset = sinf(progress * 3.14159f) * m_jumpHeight;
            ctx.position.y = m_startY - jumpOffset;
            
            // horizontal movement (linear)
            float horizontalDist = m_jumpWidth * progress * m_direction;
            ctx.position.x = m_startX + horizontalDist;
            
            if (m_elapsed >= m_duration) {
                ctx.position.y = m_startY;
                m_complete = true;
            }
        }

        void OnExit(Pet::PetContext& ctx) override {
            ctx.mood = Pet::PetMood::Neutral;
            ctx.position.y = m_startY; // Ensure we land at same height
        }

        bool IsComplete() const override { return m_complete; }
        const char* GetName() const override { return "jump"; }
        int GetPriority() const override { return 15; }

    private:
        float m_duration;
        float m_elapsed;
        bool m_complete;
        float m_startY;
        float m_startX;
        float m_jumpHeight;
        float m_jumpWidth;
        float m_direction;
    };
}
