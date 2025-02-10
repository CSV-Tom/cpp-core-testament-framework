#ifndef EVENTFRAMEWORK_EVENTDISPATCHER_HPP
#define EVENTFRAMEWORK_EVENTDISPATCHER_HPP

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>

#include "BaseEvent.hpp"

class EventDispatcher {
public:
    template <typename Event, typename Handler>
    void registerHandler(Handler&& handler) {
        using EventType = std::decay_t<Event>;
        handlers[typeid(EventType)].push_back([handler = std::forward<Handler>(handler)](const BaseEvent& event) {
            handler(static_cast<const EventType&>(event));
        });
    }

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

#endif