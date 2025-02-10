#ifndef EVENTHANDLERS_LOGEVENTHANDLER_HPP
#define EVENTHANDLERS_LOGEVENTHANDLER_HPP

#include "EventFramework/FilterEventDispatcher.hpp"
#include "EventFramework/Events/LogEvent.hpp"
#include <iostream>

class LogEventHandler {
public:
    explicit LogEventHandler(FilterEventDispatcher& dispatcher) {
        dispatcher.registerHandler<LogEvent>(
            [](const LogEvent& event) {
                std::cout << "[LogEvent] Received: " << event.message << std::endl;
            }
        );
    }

};

#endif