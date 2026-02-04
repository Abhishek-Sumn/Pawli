#pragma once
#include "Action.h"
#include <random>
#include <cmath>

namespace Actions {

    class ReactAction : public Action {
    public:
        ReactAction() 
            : m_duration(1.0f)
            , m_elapsed(0)
            , m_complete(false)
            , m_reactionType(0)
            , m_startY(0)
            , m_startX(0)
        {}

        void OnEnter(Pet::PetContext& ctx) override {
            ctx.mood = Pet::PetMood::Happy;
            m_elapsed = 0;
            m_complete = false;
            m_startY = ctx.position.y;
            m_startX = ctx.position.x;
            
            // Pick random reaction type (0-5)
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> typeDist(0, 5);
            m_reactionType = typeDist(gen);
            
            // Set animation and duration based on reaction type
            switch(m_reactionType) {
                case 0: // Quick bounce with hearts
                    ctx.currentAnimation = "react";
                    m_duration = 0.6f;
                    break;
                case 1: // Jump with love hearts
                    ctx.currentAnimation = "love";
                    m_duration = 1.0f;
                    break;
                case 2: // Curious head tilt
                    ctx.currentAnimation = "curious";
                    m_duration = 1.2f;
                    break;
                case 3: // Big yawn stretch
                    ctx.currentAnimation = "yawn";
                    m_duration = 1.5f;
                    break;
                case 4: // Wiggle with react
                    ctx.currentAnimation = "react";
                    m_duration = 0.8f;
                    break;
                case 5: // Happy spin with love
                    ctx.currentAnimation = "love";
                    m_duration = 1.2f;
                    break;
            }
        }

        void OnUpdate(Pet::PetContext& ctx, float deltaTime) override {
            m_elapsed += deltaTime;
            float progress = m_elapsed / m_duration;
            
            switch(m_reactionType) {
                case 0: // Quick bounce
                    if (progress < 0.5f) {
                        ctx.position.y = m_startY - sinf(progress * 3.14159f * 2) * 20.0f;
                    } else {
                        ctx.position.y = m_startY;
                    }
                    break;
                    
                case 1: // Jump with love 
                    ctx.position.y = m_startY - sinf(progress * 3.14159f) * 35.0f;
                    break;
                    
                case 2: // Curious - tilt left/right
                    if (progress < 0.33f) {
                        ctx.direction = Pet::PetDirection::Left;
                    } else if (progress < 0.66f) {
                        ctx.direction = Pet::PetDirection::Right;
                    } else {
                        ctx.direction = Pet::PetDirection::Left;
                    }
                    break;
                    
                case 3: // Yawn - small stretch up
                    if (progress < 0.7f) {
                        ctx.position.y = m_startY - sinf(progress / 0.7f * 3.14159f) * 10.0f;
                    }
                    break;
                    
                case 4: // Wiggle
                    ctx.position.x = m_startX + sinf(m_elapsed * 25.0f) * 3.0f;
                    break;
                    
                case 5: // Happy spin
                    if (progress < 0.25f || (progress > 0.5f && progress < 0.75f)) {
                        ctx.direction = Pet::PetDirection::Left;
                    } else {
                        ctx.direction = Pet::PetDirection::Right;
                    }
                    ctx.position.y = m_startY - sinf(progress * 6.28f) * 15.0f;
                    break;
            }
            
            if (m_elapsed >= m_duration) {
                ctx.position.y = m_startY;
                ctx.position.x = m_startX;
                m_complete = true;
            }
        }

        void OnExit(Pet::PetContext& ctx) override {
            ctx.mood = Pet::PetMood::Neutral;
            ctx.position.y = m_startY;
            ctx.position.x = m_startX;
        }

        bool IsComplete() const override { return m_complete; }
        const char* GetName() const override { return "react"; }
        int GetPriority() const override { return 10; }

    private:
        float m_duration;
        float m_elapsed;
        bool m_complete;
        int m_reactionType;
        float m_startY;
        float m_startX;
    };
}


