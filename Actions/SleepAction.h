#pragma once
#include "Action.h"

namespace Actions {

    class SleepAction : public Action {
    public:
        SleepAction(float duration = 8.0f) 
            : m_elapsed(0)
            , m_duration(duration)
        {}

        void OnEnter(Pet::PetContext& ctx) override {
            ctx.currentAnimation = "sleep";
            ctx.velocity = Utils::Vec2(0, 0);
            ctx.mood = Pet::PetMood::Sleepy;
            m_elapsed = 0;
        }

        void OnUpdate(Pet::PetContext& ctx, float deltaTime) override {
            m_elapsed += deltaTime;
        }

        void OnExit(Pet::PetContext& ctx) override {
            ctx.mood = Pet::PetMood::Neutral;
        }

        bool IsComplete() const override { return m_elapsed >= m_duration; }
        const char* GetName() const override { return "sleep"; }

    private:
        float m_elapsed;
        float m_duration;
    };
}

