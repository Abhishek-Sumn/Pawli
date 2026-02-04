#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

namespace Core {

    enum class EventType {
        DesktopChanged,
        MonitorChanged,
        CursorScreenChanged,
        InputEvent,
        ActionComplete,
        StateChanged,
        TimerTick
    };

    struct Event {
        EventType type;
        int intParam1;
        int intParam2;
        void* ptrParam;
    };

    using EventCallback = std::function<void(const Event&)>;

    class EventBus {
    public:
        using ListenerId = size_t;

        static EventBus& Get() {
            static EventBus instance;
            return instance;
        }

        ListenerId Subscribe(EventType type, EventCallback callback) {
            ListenerId id = m_nextId++;
            m_listeners[type].push_back({ id, callback });
            return id;
        }

        void Unsubscribe(ListenerId id) {
            for (auto& pair : m_listeners) {
                auto& vec = pair.second;
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [id](const Listener& l) { return l.id == id; }), vec.end());
            }
        }

        void Emit(const Event& evt) {
            auto it = m_listeners.find(evt.type);
            if (it != m_listeners.end()) {
                for (auto& listener : it->second) {
                    listener.callback(evt);
                }
            }
        }

        void Emit(EventType type) {
            Event evt;
            evt.type = type;
            evt.intParam1 = 0;
            evt.intParam2 = 0;
            evt.ptrParam = nullptr;
            Emit(evt);
        }

    private:
        EventBus() : m_nextId(1) {}

        struct Listener {
            ListenerId id;
            EventCallback callback;
        };

        std::unordered_map<EventType, std::vector<Listener>> m_listeners;
        ListenerId m_nextId;
    };
}
