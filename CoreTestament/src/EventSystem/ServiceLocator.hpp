#ifndef EVENTSYSTEM_SERVICELOCATOR_HPP
#define EVENTSYSTEM_SERVICELOCATOR_HPP


#include "EventSystem/EventManager.hpp"

class ServiceLocator {
public:
    static void provide(EventManager* manager) {
        eventManager = manager;
    }

    static EventManager& getEventManager() {
        if (!eventManager) {
            throw std::runtime_error("EventManager not provided!");
        }
        return *eventManager;
    }

private:
    static inline EventManager* eventManager = nullptr;
};

#endif