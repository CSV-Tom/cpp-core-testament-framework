#ifndef EVENTSYSTEM_EVENTMANAGER_HPP
#define EVENTSYSTEM_EVENTMANAGER_HPP

#include "EventFramework/FilterEventDispatcher.hpp"
#include "EventHandlers/LogEventHandler.hpp"

class EventManager {
public:
    EventManager() : dispatcher() {
        // Alle Event-Handler initialisieren
        logEventHandler = std::make_unique<LogEventHandler>(dispatcher);
    }

    // Zugriff auf den Dispatcher
    FilterEventDispatcher& getDispatcher() {
        return dispatcher;
    }

private:
    FilterEventDispatcher dispatcher;
    std::unique_ptr<LogEventHandler> logEventHandler;
};

#endif