#pragma once
#include "Action.h"
#include <random>

namespace Actions {

    class IdleAction : public Action {
    public:
        IdleAction() 
            : m_duration(0)
            , m_elapsed(0)
            , m_complete(false)
        {
            // Random idle duration between 3-10 seconds
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(3.0f, 10.0f);
            m_duration = dist(gen);
        }

        void OnEnter(Pet::PetContext& ctx) override {
            ctx.currentAnimation = "idle";
            ctx.velocity = Utils::Vec2(0, 0);
            m_elapsed = 0;
            m_complete = false;
        }

        void OnUpdate(Pet::PetContext& ctx, float deltaTime) override {
            m_elapsed += deltaTime;
            ctx.idleTime = m_elapsed;

            if (m_elapsed >= m_duration) {
                m_complete = true;
            }

            // Could add micro-animations here (blink, etc.)
        }

        void OnExit(Pet::PetContext& ctx) override {
            ctx.idleTime = 0;
        }

        bool IsComplete() const override { return m_complete; }
        const char* GetName() const override { return "idle"; }

    private:
        float m_duration;
        float m_elapsed;
        bool m_complete;
    };
}
