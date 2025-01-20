#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>

#include "BaseEvent.hpp"

class FilterEventDispatcher {
public:
    // Register a handler with an optional filter
    template <typename Event, typename Handler>
    void registerHandler(Handler&& handler, std::function<bool(const Event&)> filter = nullptr) {
        using EventType = std::decay_t<Event>;
        handlers[typeid(EventType)].push_back([handler = std::forward<Handler>(handler), filter](const BaseEvent& event) {
            const auto& specificEvent = static_cast<const EventType&>(event);
            if (!filter || filter(specificEvent)) {
                handler(specificEvent);
            }
        });
    }

    // Dispatch an event
    template <typename Event>
    void dispatch(const Event& event) {
        using EventType = std::decay_t<Event>;
        auto it = handlers.find(typeid(EventType));
        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const BaseEvent&)>>> handlers;
};