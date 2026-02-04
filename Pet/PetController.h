#pragma once
#include "PetContext.h"
#include "PetStateMachine.h"
#include "../Actions/Action.h"
#include "../Actions/IdleAction.h"
#include "../Actions/WalkAction.h"
#include "../Actions/SleepAction.h"
#include "../Actions/ReactAction.h"
#include "../Actions/JumpAction.h"
#include <memory>
#include <random>

namespace Pet {

    class PetController {
    public:
        PetController();
        ~PetController();

        void Initialize();
        void Update(float deltaTime);

        PetContext& GetContext() { return m_context; }
        const PetContext& GetContext() const { return m_context; }

        // Handle input events
        void OnLeftClick();
        void OnRightClickStart();
        void OnRightClickEnd();
        void OnDrag(float x, float y);

        // Set movement bounds (from monitor)
        void SetMovementBounds(const RECT& bounds);

    private:
        void ChooseNextAction();
        void TransitionTo(const std::string& actionName);
        Actions::Action* CreateAction(const std::string& name);

        PetContext m_context;
        PetStateMachine m_stateMachine;
        std::unique_ptr<Actions::Action> m_currentAction;
        
        // Config values
        float m_walkChance;
        float m_sleepChance;
        float m_walkSpeed;
        int m_walkCount;           // How many walks since last sleep
        int m_walksBeforeSleep;    // How many walks before going to sleep
        
        // Multi-display tracking
        int m_lastMonitorIndex;    // Track which monitor we were on
        bool m_isJumping;          // Are we doing a display crossing jump?

        // Termination feature
        int m_consecutiveRightClicks;
        float m_timeSinceLastRightClick;
    };
}
