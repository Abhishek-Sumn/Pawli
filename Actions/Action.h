#pragma once
#include "PetContext.h"

namespace Actions {

    class Action {
    public:
        virtual ~Action() = default;

        // Called when action starts
        virtual void OnEnter(Pet::PetContext& ctx) = 0;

        // Called every frame while action is active
        // Returns true if action wants to continue, false if complete
        virtual void OnUpdate(Pet::PetContext& ctx, float deltaTime) = 0;

        virtual void OnExit(Pet::PetContext& ctx) = 0;

        virtual bool IsComplete() const = 0;

        virtual const char* GetName() const = 0;

        // Get the animation name for this action
        virtual const char* GetAnimation() const { return GetName(); }

        // Priority (higher = can interrupt lower)
        virtual int GetPriority() const { return 0; }
    };
}
