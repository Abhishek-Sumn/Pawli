#pragma once
#include "Action.h"

namespace Actions {

    class PickUpAction : public Action {
    public:
        PickUpAction() : m_complete(false) {}

        void OnEnter(Pet::PetContext& ctx) override {
            ctx.currentAnimation = "pickedup";
            ctx.isPickedUp = true;
            ctx.mood = Pet::PetMood::Excited;
            m_complete = false;
        }

        void OnUpdate(Pet::PetContext& ctx, float deltaTime) override {
            // Position is updated externally via drag
            // This action just maintains the picked-up state
        }

        void OnExit(Pet::PetContext& ctx) override {
            ctx.isPickedUp = false;
            ctx.mood = Pet::PetMood::Neutral;
        }

        bool IsComplete() const override { return m_complete; }
        const char* GetName() const override { return "pickedup"; }
        int GetPriority() const override { return 100; } // Highest priority
        
        void Release() { m_complete = true; }

    private:
        bool m_complete;
    };
}
