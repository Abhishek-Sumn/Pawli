#include "PetController.h"
#include "../Desktop/MonitorManager.h"
#include "../Utils/Logger.h"

namespace Pet {

    PetController::PetController()
        : m_walkChance(0.7f)       // Higher chance to walk
        , m_sleepChance(0.1f)
        , m_walkSpeed(150.0f)      // Faster movement
        , m_walkCount(0)
        , m_walksBeforeSleep(5)    // Sleep after 5 walks
        , m_lastMonitorIndex(-1)   // Unknown at start
        , m_isJumping(false)
        , m_consecutiveRightClicks(0)
        , m_timeSinceLastRightClick(0.0f)
    {}

    PetController::~PetController() {}

    void PetController::Initialize() {
        // Start with kitty sleeping for a short nap, then she wakes up
        m_stateMachine.SetState("idle");  // Reset first to allow transition
        m_currentAction.reset(new Actions::SleepAction(4.0f));  // 4 sec startup sleep
        m_currentAction->OnEnter(m_context);
        m_stateMachine.SetState("idle");  // Will transition to idle when done
    }

    void PetController::Update(float deltaTime) {
        if (!m_context.isPickedUp) {
            m_context.idleTime += deltaTime;
        }

        // Track time for right click termination
        m_timeSinceLastRightClick += deltaTime;
        if (m_timeSinceLastRightClick > 0.5f) { // Reset count if more than 0.5s between clicks
             m_consecutiveRightClicks = 0;
        }

        if (m_context.isPickedUp) {
            // Don't update actions while being dragged
            return;
        }

        if (m_currentAction) {
            m_currentAction->OnUpdate(m_context, deltaTime);

            // Check for monitor crossing during walk
            if (std::string(m_currentAction->GetName()) == "walk" && !m_isJumping) {
                POINT pt;
                pt.x = (LONG)m_context.position.x + 100;  // Center of sprite
                pt.y = (LONG)m_context.position.y + 100;
                
                Desktop::MonitorManager monitorMgr;
                int currentMonitor = monitorMgr.GetMonitorIndex(pt);
                
                if (m_lastMonitorIndex == -1) {
                    m_lastMonitorIndex = currentMonitor;
                } else if (currentMonitor != m_lastMonitorIndex && currentMonitor != -1) {
                    // Crossed to a different monitor - JUMP!
                    Utils::Logger::Log(L"Display crossing detected! Monitor %d -> %d", m_lastMonitorIndex, currentMonitor);
                    m_lastMonitorIndex = currentMonitor;
                    m_isJumping = true;
                    
                    // Interrupt walk and do jump
                    m_currentAction->OnExit(m_context);
                    m_currentAction.reset(new Actions::JumpAction());
                    m_currentAction->OnEnter(m_context);
                    return;
                }
            }
            
            // Reset jumping flag when jump completes
            if (m_isJumping && m_currentAction->IsComplete()) {
                m_isJumping = false;
            }

            if (m_currentAction->IsComplete()) {
                ChooseNextAction();
            }
        }

        m_context.stateTime += deltaTime;
    }

    void PetController::OnLeftClick() {
        Utils::Logger::Log(L"PetController: Left click - React!");
        // Trigger react action on left click
        if (m_currentAction) {
            m_currentAction->OnExit(m_context);
        }
        TransitionTo("react");
    }

    void PetController::OnRightClickStart() {
        Utils::Logger::Log(L"PetController: Pick up start");
        // Start dragging
        m_context.isPickedUp = true;
        TransitionTo("pickedup"); // New state for being held

        // Termination logic: 5 consecutive clicks
        if (m_timeSinceLastRightClick < 0.5f) {
            m_consecutiveRightClicks++;
        } else {
            m_consecutiveRightClicks = 1;
        }
        m_timeSinceLastRightClick = 0.0f;

        Utils::Logger::Log(L"Right clicks: %d", m_consecutiveRightClicks);

        if (m_consecutiveRightClicks >= 5) {
             Utils::Logger::Log(L"Terminating application via secret code...");
             PostQuitMessage(0);
        }
    }

    void PetController::OnRightClickEnd() {
        Utils::Logger::Log(L"PetController: Dropped at (%.0f, %.0f)", m_context.position.x, m_context.position.y);
        m_context.isPickedUp = false;
        m_context.ClampToBounds();
        
        // Transition back to idle
        m_stateMachine.SetState("placed");
        TransitionTo("idle");
    }

    void PetController::OnDrag(float x, float y) {
        if (m_context.isPickedUp) {
            m_context.position.x = x;
            m_context.position.y = y;
        }
    }

    void PetController::SetMovementBounds(const RECT& bounds) {
        m_context.movementBounds = bounds;
        Utils::Logger::Log(L"Movement bounds set: (%d, %d) - (%d, %d)",
            bounds.left, bounds.top, bounds.right, bounds.bottom);
    }

    void PetController::ChooseNextAction() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(gen);

        std::string currentState = m_stateMachine.GetCurrentState();

        if (currentState == "walk") {
            m_walkCount++;
            
            // After several walks, go to sleep at current position
            if (m_walkCount >= m_walksBeforeSleep) {
                m_walkCount = 0;
                // Sleep where kitty is - no teleporting
                TransitionTo("sleep");
                return;
            }
            
            // Otherwise, short idle then walk again
            TransitionTo("idle");
        } else if (currentState == "idle") {
            // From idle, high chance to walk
            if (roll < m_walkChance) {
                TransitionTo("walk");
            } else {
                TransitionTo("idle");  // Stay idle a bit longer
            }
        } else if (currentState == "sleep") {
            // Wake up after sleeping, reset walk count
            m_walkCount = 0;
            TransitionTo("idle");
        } else {
            TransitionTo("idle");
        }
    }

    void PetController::TransitionTo(const std::string& actionName) {
        if (m_currentAction) {
            m_currentAction->OnExit(m_context);
        }

        // Update state machine (if valid transition)
        if (actionName != m_stateMachine.GetCurrentState()) {
            if (m_stateMachine.CanTransition(actionName)) {
                m_stateMachine.SetState(actionName);
            }
        }

        // Create new action
        m_currentAction.reset(CreateAction(actionName));
        
        if (m_currentAction) {
            m_currentAction->OnEnter(m_context);
            m_context.stateTime = 0;
        }
    }

    Actions::Action* PetController::CreateAction(const std::string& name) {
        if (name == "idle") {
            return new Actions::IdleAction();
        } else if (name == "walk") {
            return new Actions::WalkAction(m_walkSpeed);
        } else if (name == "sleep") {
            return new Actions::SleepAction(12.0f);  // Sleep for 12 seconds
        } else if (name == "react") {
            return new Actions::ReactAction();  // Random reactions!
        } else if (name == "jump") {
            return new Actions::JumpAction();  // Display crossing jump!
        }
        
        return new Actions::IdleAction(); // Default fallback
    }
}
