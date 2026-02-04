#pragma once
#include "PetContext.h"
#include "../Actions/Action.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Pet {

    class PetStateMachine {
    public:
        PetStateMachine();
        ~PetStateMachine();

        // Register allowed transitions
        void AddTransition(const std::string& from, const std::string& to);
        
        // Check if transition is allowed
        bool CanTransition(const std::string& to) const;

        // Set current state
        void SetState(const std::string& state);
        
        // Get current state name
        const std::string& GetCurrentState() const { return m_currentState; }

        // Get possible transitions from current state
        const std::vector<std::string>& GetPossibleTransitions() const;

    private:
        std::string m_currentState;
        std::unordered_map<std::string, std::vector<std::string>> m_transitions;
        std::vector<std::string> m_emptyVec; // For returning empty
    };
}
