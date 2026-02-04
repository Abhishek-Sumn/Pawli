#pragma once
#include "../Utils/Math.h"
#include <windows.h>
#include <string>

namespace Pet {

    enum class PetMood {
        Happy,
        Neutral,
        Sleepy,
        Excited
    };

    enum class PetDirection {
        Left,
        Right
    };

    struct PetContext {
        // Position and movement
        Utils::Vec2 position;
        Utils::Vec2 velocity;
        Utils::Vec2 targetPosition;
        
        // Movement bounds (current monitor work area)
        RECT movementBounds;
        
        // State
        PetMood mood;
        PetDirection direction;
        bool isPickedUp;
        bool isOnGround;
        
        // Animation
        std::string currentAnimation;
        int currentFrame;
        float frameTime;
        
        // Timers
        float idleTime;       // Time spent in current idle
        float stateTime;      // Time spent in current state

        PetContext() 
            : position(100, 100)
            , velocity(0, 0)
            , targetPosition(100, 100)
            , mood(PetMood::Neutral)
            , direction(PetDirection::Right)
            , isPickedUp(false)
            , isOnGround(true)
            , currentAnimation("idle")
            , currentFrame(0)
            , frameTime(0)
            , idleTime(0)
            , stateTime(0)
        {
            movementBounds = { 0, 0, 1920, 1080 }; // Default, will be updated
        }

        void ClampToBounds() {
            if (position.x < movementBounds.left) position.x = (float)movementBounds.left;
            if (position.x > movementBounds.right - 64) position.x = (float)(movementBounds.right - 64); // Assuming 64px sprite
            if (position.y < movementBounds.top) position.y = (float)movementBounds.top;
            if (position.y > movementBounds.bottom - 64) position.y = (float)(movementBounds.bottom - 64);
        }
    };
}
