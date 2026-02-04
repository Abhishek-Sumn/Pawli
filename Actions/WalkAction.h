#pragma once
#include "Action.h"
#include "../Desktop/MonitorManager.h"
#include <random>

namespace Actions {

    class WalkAction : public Action {
    public:
        WalkAction(float speed = 150.0f)  // Fast movement
            : m_speed(speed)
            , m_complete(false)
            , m_targetX(0)
            , m_targetY(0)
        {}

        void OnEnter(Pet::PetContext& ctx) override {
            ctx.currentAnimation = "walk";
            m_complete = false;

            std::random_device rd;
            std::mt19937 gen(rd());
            

            // Use MonitorManager to pick a random monitor to visit
            Desktop::MonitorManager monitorMgr; // Refreshes on construction
            const auto& monitors = monitorMgr.GetAllMonitors();
            
            if (!monitors.empty()) {
                // Pick a random monitor
                std::uniform_int_distribution<size_t> monDist(0, monitors.size() - 1);
                const auto& targetMonitor = monitors[monDist(gen)];
                
                // Pick a target within that monitor's work area (with padding)
                float padding = 100.0f;
                float minX = (float)targetMonitor.workArea.left + padding;
                float maxX = (float)targetMonitor.workArea.right - padding;
                float minY = (float)targetMonitor.workArea.top + padding;
                float maxY = (float)targetMonitor.workArea.bottom - padding;
                
                // Ensure valid ranges
                if (minX > maxX) minX = maxX = (float)targetMonitor.workArea.left;
                if (minY > maxY) minY = maxY = (float)targetMonitor.workArea.top;

                std::uniform_real_distribution<float> distX(minX, maxX);
                std::uniform_real_distribution<float> distY(minY, maxY);
                
                m_targetX = distX(gen);
                m_targetY = distY(gen);
            } else {
                // Fallback to primary screen center if no monitors found (shouldn't happen)
                m_targetX = 100.0f; 
                m_targetY = 100.0f;
            }

            // Set direction based on target X
            ctx.direction = (m_targetX > ctx.position.x) ? Pet::PetDirection::Right : Pet::PetDirection::Left;
        }

        void OnUpdate(Pet::PetContext& ctx, float deltaTime) override {
            float dx = m_targetX - ctx.position.x;
            float dy = m_targetY - ctx.position.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < 5.0f) {
                ctx.position.x = m_targetX;
                ctx.position.y = m_targetY;
                ctx.velocity.x = 0;
                ctx.velocity.y = 0;
                m_complete = true;
                return;
            }

            float dirX = dx / distance;
            float dirY = dy / distance;
            float step = m_speed * deltaTime;
            if (step > distance) step = distance;

            ctx.velocity.x = dirX * m_speed;
            ctx.velocity.y = dirY * m_speed;
            ctx.position.x += dirX * step;
            ctx.position.y += dirY * step;

            ctx.direction = dx > 0 ? Pet::PetDirection::Right : Pet::PetDirection::Left;
        }

        void OnExit(Pet::PetContext& ctx) override {
            ctx.velocity.x = 0;
            ctx.velocity.y = 0;
        }

        bool IsComplete() const override { return m_complete; }
        const char* GetName() const override { return "walk"; }

    private:
        float m_speed;
        bool m_complete;
        float m_targetX;
        float m_targetY;
    };
}

