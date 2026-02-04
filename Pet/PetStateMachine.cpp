#include "PetStateMachine.h"
#include "../Utils/Logger.h"

namespace Pet {

    PetStateMachine::PetStateMachine() : m_currentState("idle") {
        // Default transitions
        AddTransition("idle", "walk");
        AddTransition("idle", "sleep");
        AddTransition("idle", "react");
        AddTransition("idle", "pickedup");
        
        AddTransition("walk", "idle");
        AddTransition("walk", "sleep");    // Can sleep after walking
        AddTransition("walk", "react");    // Can react while walking
        AddTransition("walk", "pickedup");
        
        AddTransition("sleep", "idle");
        AddTransition("sleep", "react");   // Can wake up with reaction
        AddTransition("sleep", "pickedup");
        
        AddTransition("react", "idle");
        
        // Jump state (display crossing)
        AddTransition("walk", "jump");
        AddTransition("idle", "jump");
        AddTransition("jump", "idle");
        AddTransition("jump", "walk");
        
        AddTransition("pickedup", "placed");
        AddTransition("placed", "idle");
    }

    PetStateMachine::~PetStateMachine() {}

    void PetStateMachine::AddTransition(const std::string& from, const std::string& to) {
        m_transitions[from].push_back(to);
    }

    bool PetStateMachine::CanTransition(const std::string& to) const {
        auto it = m_transitions.find(m_currentState);
        if (it == m_transitions.end()) {
            return false;
        }

        for (const auto& state : it->second) {
            if (state == to) {
                return true;
            }
        }
        return false;
    }

    void PetStateMachine::SetState(const std::string& state) {
        if (CanTransition(state) || m_currentState.empty()) {
            Utils::Logger::Log(L"State: %S -> %S", m_currentState.c_str(), state.c_str());
            m_currentState = state;
        } else {
            Utils::Logger::Log(L"Invalid transition: %S -> %S", m_currentState.c_str(), state.c_str());
        }
    }

    const std::vector<std::string>& PetStateMachine::GetPossibleTransitions() const {
        auto it = m_transitions.find(m_currentState);
        if (it != m_transitions.end()) {
            return it->second;
        }
        return m_emptyVec;
    }
}
